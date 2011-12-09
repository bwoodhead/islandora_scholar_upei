<?php

/**
 * @file
 * Php you suck so badly, the array_filter function should just take extra parameters like array_walk...
 */
class AutoCompleteFilterScholar {

  protected $string;

  function __construct($string) {
    $this->string = strtolower($string);
  }

  function __invoke($content_model) {
    return (preg_match("/^{$this->string}/i", $content_model) !== 0) ? TRUE : FALSE;
  }

}

/**
 * Autocomplete researchers from a solr query..
 * 
 * @param string $string The string of characters to match with the results
 * @return string List of results in JSON format
 */
function islandora_autocomplete_researchers_given($string = '') {
  $content_models = islandora_scholar_get_values('rels.hasModel:researcherCModel', 'mads.given');
  // @todo find an alternative solution to array filter because of earlier versions 5.2
  $output = array_filter($content_models, new AutoCompleteFilterScholar($string));
  $output[$string] = $string;
//  var_dump($output);
  return drupal_json($output);
}

function islandora_autocomplete_researchers_family($string = '') {
  $content_models = islandora_scholar_get_values('rels.hasModel:researcherCModel', 'mads.family');
  // @todo find an alternative solution to array filter because of earlier versions 5.2
  $output = array_filter($content_models, new AutoCompleteFilterScholar($string));
  $output[$string] = $string;
//  var_dump($output);
  return drupal_json($output);
}

/**
 * Autocomplete institutions..
 * 
 * @param string $string The string of characters to match with the results
 * @return string List of results in JSON format
 */
function islandora_autocomplete_departments($string = '') {
  $content_models = islandora_scholar_get_values('rels.hasModel:researcherCModel', 'mads.topic');
  // @todo find an alternative solution to array filter because of earlier versions 5.2
  if ($content_models == NULL) {
    drupal_set_message('Solr is not responding!', 'error');
    break;
  }
  else {
  $output = array_filter($content_models, new AutoCompleteFilterScholar($string));
  $output[$string] = $string;
  return drupal_json($output);
  }
}

/**
 *
 * @param type $query Criteria to search for as a solr query
 * @param type $results The field that you want returned
 * @return type An array of results with the key and element being the same 
 */
function islandora_scholar_get_values($query, $results) {
  
  $solr_url = variable_get('fedora_solr_search_url', 'http://localhost:8080/solr');
  
  if (is_array($results)) {
    $result_string = implode(',', $results);
    $query = file_get_contents($solr_url . '/select?q=' . $query . '&fl=' . $result_string);
  }
  else {
  $query = file_get_contents($solr_url . '/select?q=' . $query . '&fl=' . $results);
  }
  $result = new SimpleXMLElement($query);
  $researchers = $result->result;
//  var_dump($researchers);
  $list = array();
  
  foreach ($researchers->children() as $researcher) {
    $value = (string)$researcher->arr[0]->str;
    $list[$value] = $value;
  }
//  var_dump($list);
  return $list;
}