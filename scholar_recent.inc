<?php

/**
 * @file
 * @param type $form_state
 * @return string 
 */

function scholar_recent_items($form_state) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
    module_load_include('inc', 'Fedora_Repository', 'CollectionClass');
    $query_string = 'select $object $modified from <#ri> where $object <fedora-model:hasModel> <info:fedora/ir:citationCModel> and $object <fedora-view:lastModifiedDate> $modified and $modified <mulgara:after> \'2010-02-22T12:54:59.265Z\'^^<xml-schema:dateTime> in <#xsd> order by $modified desc';
    $query_string = htmlentities(urlencode($query_string));
    $content = '';
    $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url .= "?type=tuples&flush=TRUE&format=Sparql&limit=10&offset=0&lang=itql&stream=on&query=" . $query_string;
    $content .= do_curl($url);
    if ($content == '') {
      drupal_set_message(t('The fedora server is not responding!'));
      return FALSE;
    }
    $result = new SimpleXMLElement($content);
  
//    var_dump($result);
  $entries = $result->results->result;
  
  $list = '<ul>';
  
  foreach ($entries as $entry) {
    $pid = explode('/', $entry->object['uri']);
    $collection_helper = new CollectionClass();
    $xmlstr = $collection_helper->getStream($pid[1], "MODS");
    if ($xmlstr) {
    $xml = new SimpleXMLElement($xmlstr);
    $list .= '<li>' . l($xml->titleInfo->title, 'fedora/repository/' . $pid[1]) .'</li>';
    }
    else {
      $list .= '<li>' . l(t('Title not found'), 'fedora/repository/' . $pid[1]) .'</li>';
    }
  }
 
  
  $list .= '</ul>';
  $form = array();
  
//  var_dump($content);
  
  $form['title'] = array(
    '#value' => $list,
  );
  
  return $form;
  
}