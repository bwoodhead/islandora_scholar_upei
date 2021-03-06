<?php

/**
 * @file
 */

/**
 * Cron function to remove the policy datastream from an object if the 
 * datastream has the label 'Embargo policy' and the mods.embargo_date
 * index field has expired in the last week. If the cron job is set up
 * properly then we sould only ever need to check the last week. Checking
 * further back would increase the number of objects that would need
 * to be checked.
 */
function scholar_cron_embargo_check() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $solr_query = 'http://localhost:8080/solr/select/?q=mods.embargo_date:[NOW-7DAY%20TO%20NOW]&fl=PID&rows=10000';
  $solr_result = @file_get_contents($solr_query);

  if ($solr_result != NULL) {
    $xml = new SimpleXMLElement($solr_result);
    $array = $xml->xpath('//doc');
    $array = array_filter($array);
    $pids = array();
    foreach ($array as $arr) {
      $pid = (string) $arr[0]->str;
      $pids[] = $pid;
      $item = new Fedora_Item($pid);
      $ds_array = $item->get_datastreams_list_as_array();
      foreach ($ds_array as $key => $ds) {
        if ($key == 'POLICY' && $ds['label'] == 'Embargo policy') {
          $purge_status = $item->purge_datastream('POLICY');
          if ($purge_status != NULL) {
            drupal_set_message(t('Embargo policy removed for @pid', array('@pid' => $pid)));
          }
        }
      }
    }
  }
}