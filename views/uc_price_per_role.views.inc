<?php
/**
 * @file
 * Expose UC Price per Role prices to Views.
 */

/**
 * Implementation of hook_views_data().
 */
function uc_price_per_role_views_data() {

  $data['uc_price_per_role_prices']['table']['group']  = t('Product');

  $data['uc_price_per_role_prices']['table']['join']['node'] = array(
    'type' => 'inner',
    'left_field' => 'nid',
    'field' => 'nid',
  );
  $data['uc_price_per_role_prices']['table']['join']['node_revision'] = array(
    'type' => 'inner',
    'left_field' => 'vid',
    'field' => 'vid',
  );

  $data['uc_price_per_role_prices']['table']['default_relationship']['node'] = array(
    'table' => 'node',
    'field' => 'vid',
  );
  $data['uc_price_per_role_prices']['table']['default_relationship']['node_revision'] = array(
    'table' => 'node_revision',
    'field' => 'vid',
  );

  $data['uc_price_per_role_prices']['rid'] = array(
    'title' => t('Role ID'),
    'help' => t('Role ID for a price per role'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  $data['uc_price_per_role_prices']['price'] = array(
    'title' => t('Role price'),
    'help' => t('Role price for a price per role'),
    'field' => array(
      'handler' => 'uc_product_handler_field_price',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'uc_product_handler_field_price',
    ),
  );

  // More uc_product fields

  $data['uc_products']['particular_role_price'] = array(
    'real field' => 'sell_price',
    'title' => t('Particular role price'),
    'help' => t('A particular role price as determined by uc_price_per_role, set in handler options.'),
    'field' => array(
      'handler' => 'uc_price_per_role_handler_field_role_price',
      'no group by' => TRUE,
     ),
  );

  return $data;
}

