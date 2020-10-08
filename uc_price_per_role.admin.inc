<?php
/**
 * @file
 * Price-per-role administrative pages
 */


/**
 * Configuration settings form.
 *
 * @see theme_uc_price_per_role_settings_form()
 */
function uc_price_per_role_settings_form() {
  $config = config('uc_price_per_role.settings');
  $enabled = $config->get('enabled');
  $weights = $config->get('weights');
  $roles = user_roles();

  foreach (array_keys($roles) as $role) {
    if (!isset($weights[$role])) {
      $weights[$role] = 0;
    }
  }
  asort($weights);

  $form['fields']['#tree'] = TRUE;
  foreach ($weights as $role => $weight) {
    $form['fields'][$role]['role'] = array(
      '#markup' => $roles[$role],
    );
    $form['fields'][$role]['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($enabled[$role]),
    );
    $form['fields'][$role]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 5,
      '#default_value' => $weight,
      '#attributes' => array('class' => array('role-weight')),
    );
  }

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}


/**
 * Theme the configuration settings form.
 *
 * @see uc_price_per_role_settings_form()
 */
function theme_uc_price_per_role_settings_form($variables) {
  $form = $variables['form'];

  $output = '<p>' . t('Enable roles that require separate pricing. For users with more than one matching role, the lightest weight role that has a price available will be used.') . '</p>';

  $header = array(t('Enable'), t('Role'), t('Weight'));
  $rows = array();

  if (!empty($form['fields'])) {
    foreach (element_children($form['fields']) as $field) {
      $row = array();
      $row[] = backdrop_render($form['fields'][$field]['enabled']);
      $row[] = backdrop_render($form['fields'][$field]['role']);
      $row[] = backdrop_render($form['fields'][$field]['weight']);
      $rows[] = array(
        'data' => $row,
        'class' => array('draggable'),
      );
    }
  }

  backdrop_add_tabledrag('uc-price-per-roles', 'order', 'sibling', 'role-weight');
  $output .= theme(
    'table',
    array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'uc-price-per-roles'),
  )
  );

  $output .= backdrop_render_children($form);

  return $output;
}


/**
 * Submit function for the settings form.
 *
 * @see uc_price_per_role_settings_form()
 */
function uc_price_per_role_settings_form_submit($form, &$form_state) {
  $enabled = array();
  $weights = array();

  foreach ($form_state['values']['fields'] as $role => $field) {
    $enabled[$role] = $field['enabled'];
    $weights[$role] = $field['weight'];
  }

  config_set('uc_price_per_role.settings', 'enabled', $enabled);
  config_set('uc_price_per_role.settings', 'weights', $weights);
}


/**
 * Form definition to edit option prices.
 *
 * @see theme_uc_price_per_role_option_prices_form()
 */
function uc_price_per_role_option_prices_form($form, &$form_state, $product) {
  backdrop_set_title(check_plain($product->title));

  $role_prices = uc_price_per_role_load_option_prices($product->nid);
  $enabled = array_keys(array_filter(config_get('uc_price_per_role.settings', 'enabled')));

  $form['help'] = array(
    '#markup' => t('Leave any box blank to use the default price for the option.'),
  );

  foreach (uc_product_get_attributes($product->nid) as $aid => $attribute) {
    $form['attributes'][$aid]['name'] = array(
      '#markup' => $attribute->name,
    );
    $form['attributes'][$aid]['aid'] = array(
      '#type' => 'hidden',
      '#value' => $attribute->aid,
    );
    $form['attributes'][$aid]['ordering'] = array(
      '#type' => 'value',
      '#value' => $attribute->ordering,
    );

    $form['attributes'][$aid]['options'] = array('#weight' => 2);

    $base_attr = uc_attribute_load($attribute->aid);

    if ($base_attr->options) {
      $result = db_query("
        SELECT ao.aid, ao.oid, ao.name, ao.price AS default_price, ao.ordering AS default_ordering, po.price, po.ordering, po.ordering IS NULL AS null_order FROM {uc_attribute_options} AS ao
        LEFT JOIN {uc_product_options} AS po ON ao.oid = po.oid AND po.nid = :nid
        WHERE aid = :aid ORDER BY null_order, po.ordering, default_ordering, ao.name", array(':nid' => $product->nid, ':aid' => $attribute->aid));

      foreach ($result as $option) {
        $oid = $option->oid;

        $context['revision'] = 'themed';
        $context['subject']['option'] = $option;
        $price = is_null($option->price) ? $option->default_price : $option->price;

        $form['attributes'][$aid]['options'][$oid]['name'] = array(
          '#markup' => $option->name,
        );

        $form['attributes'][$aid]['options'][$oid]['price'] = array(
          '#markup' => uc_currency_format($price),
        );

        // Include unformatted original price to simplify form alteration.
        $form['attributes'][$aid]['options'][$oid]['price_value'] = array(
          '#type' => 'value',
          '#value' => $price,
        );

        foreach ($enabled as $role) {
          $form['attributes'][$aid]['options'][$oid]['role_prices'][$role] = array(
            '#type' => 'textfield',
            '#default_value' => isset($role_prices[$oid][$role]) ? uc_store_format_price_field_value($role_prices[$oid][$role]) : '',
            '#size' => 6,
          );
        }
      }
    }
    else {
      $form['attributes'][$aid]['default'] = array(
        '#value' => t('This attribute does not have any options.'),
      );
    }
  }

  if (!empty($form['attributes'])) {
    $form['attributes']['#tree'] = TRUE;

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
      '#weight' => 10,
    );
  }

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $product->nid,
  );

  return $form;
}


/**
 * Display the option form.
 *
 * @ingroup themeable
 * @see uc_price_per_role_option_prices_form()
 */
function theme_uc_price_per_role_option_prices_form($variables) {
  $form = $variables['form'];

  $roles = user_roles();
  $enabled = array_keys(array_filter(config_get('uc_price_per_role.settings', 'enabled')));

  $header = array(t('Attribute'), t('Option'), t('Default price'));
  foreach ($enabled as $role) {
    $header[] = t('%role price', array('%role' => $roles[$role]));
  }

  if (!empty($form['attributes'])) {
    foreach (element_children($form['attributes']) as $key) {
      $row = array();
      $row[] = array(
        'data' => backdrop_render($form['attributes'][$key]['aid']) . backdrop_render($form['attributes'][$key]['name']),
        'class' => 'attribute',
      );
      if (!empty($form['attributes'][$key]['options']) && $children = element_children($form['attributes'][$key]['options'])) {
        foreach ($children as $oid) {
          $row[] = backdrop_render($form['attributes'][$key]['options'][$oid]['name']);
          $row[] = backdrop_render($form['attributes'][$key]['options'][$oid]['price']);
          foreach ($enabled as $role) {
            $row[] = backdrop_render($form['attributes'][$key]['options'][$oid]['role_prices'][$role]);
          }

          $rows[] = $row;
          $row = array('');
        }
        unset($form['attributes'][$key]['default']);
      }
      else {
        $row[] = array(
          'data' => backdrop_render($form['attributes'][$key]['default']),
          'colspan' => count($enabled) + 3,
        );
        $rows[] = $row;
      }

      $rows[] = array(array(
          'data' => '<hr />',
          'colspan' => count($enabled) + 3,
        ));
    }
  }

  if (count($rows) == 0) {
    $rows[] = array(
      array(
        'data' => t('This product does not have any attributes.'),
        'colspan' => count($enabled) + 3,
      ),
    );
  }

  $output = backdrop_render($form['help'])
          . theme(
            'table',
            array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'product_attributes'),
  )
          )
          . backdrop_render_children($form);

  return $output;
}


/**
 * Form submit handler to save option prices.
 */
function uc_price_per_role_option_prices_form_submit($form_id, &$form_state) {
  foreach ($form_state['values']['attributes'] as $attribute) {
    if (is_array($attribute['options'])) {
      foreach ($attribute['options'] as $oid => $option) {
        db_query("DELETE FROM {uc_price_per_role_option_prices} WHERE nid = :nid AND oid = :oid", array(':nid' => $form_state['values']['nid'], ':oid' => $oid));

        foreach ($option['role_prices'] as $role => $price) {
          if (is_numeric($price)) {
            $data = array(
              'nid' => $form_state['values']['nid'],
              'oid' => $oid,
              'role' => $role,
              'price' => $price,
            );
            backdrop_write_record('uc_price_per_role_option_prices', $data);
          }
        }
      }
    }
  }

  backdrop_set_message(t('The option prices have been saved.'));
}


