<?php

/**
 * @file
 * Views handler: Particular role price field.
 */

/**
 * Returns a formatted price value for a particular role to display in the View.
 */
class uc_price_per_role_handler_field_role_price extends uc_product_handler_field_price {

  /**
   * Overrides views_handler::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['role'] = array(0 => t('Applicable price'));
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $roles = user_roles();
    $priced_roles = variable_get('uc_price_per_role_enabled', array());
    $role_options = array(
      0 => t('Applicable price'),
    );
    foreach ($priced_roles as $rid => $value) {
      if ($value) {
        $role_options[$rid] = $roles[$rid];
      }
    }
    $options = $this->options;
    $form['role'] =  array(
      '#title' => t('Role'),
      '#description' => t('Choose the role price to display. "Applicable price" takes into account the user role and UC Price per Role settings, other settings return the individual role prices.'),
      '#type' => 'select',
      '#options' => $role_options,
      '#default_value' => $options['role'],
      '#weight' => 1,
    );
  }


  /**
   * Overrides views_handler_field::pre_render()
   */
   function pre_render(&$values) {
      $rid = $this->options['role'];
      // Since this field is an alias of sell_price, we need a new alias to hold
      // the altered field values. And the alias has to be unique for each value
      // of $rid because we might have multiple columns using this field handler
      // for different role settings.
      $new_alias = $this->field_alias . '_' . $rid;
      foreach ($values as &$value) {
       $nid = $value->nid;
       if ($rid == 0) {
          // Applicable price
          $result = db_query('
            SELECT rid, price
            FROM {uc_price_per_role_prices}
            WHERE nid = :nid
            ', array(':nid' => $nid));
          $prices = array();
          foreach ($result as $row) {
            $prices[$row->rid] = $row->price;
          }
          $role_price = uc_price_per_role_find_price($prices);
          if ($role_price !== FALSE) {
            $value->{$new_alias} = $role_price;
          }
          else {
            // Per uc_price_per_role_node_load(), the default price with no role
            // price set is the sell_price, which is the original value of this
            // field.
            $value->{$new_alias} = $value->{$this->field_alias};
         }
        }
        else {
          // Particular role price
          $result = db_query('
            SELECT price
            FROM {uc_price_per_role_prices}
            WHERE nid = :nid
            AND rid = :rid
            ', array(':nid' => $nid, ':rid' => $rid))
            ->fetchObject();
          if ($result) {
            $value->{$new_alias} = $result->price;
          }
          else {
            // Unlike for "Applicable price", if we specified a role price and
            // there's no role price set, we shouldn't return a default price.
            $value->{$new_alias} = NULL;
          }
        }
      }
      $this->field_alias = $new_alias;
   }
}
