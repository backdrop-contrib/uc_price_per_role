<?php
/**
 * @file
 * Feeds mapping implementation for the UC Price Per Role module.
 */

/**
 * Implements hook_feeds_processor_targets_alter().
 */
function uc_price_per_role_feeds_processor_targets_alter(&$targets, $entity_type, $bundle) {
  $config = config('uc_price_per_role.settings');
  $enabled = $config->get('enabled');
  foreach (user_roles() as $role => $role_name) {
    if (!empty($enabled[$role])) {
      $targets['role_prices_' . $role] = array(
        'name' => t('UC: Price per role for !role', array('!role' => $role_name)),
        'callback' => 'uc_price_per_role_feeds_set_target',
      );
    }
  }

}

/**
 * Callback function to set value of a price per role.
 */
function uc_price_per_role_feeds_set_target (FeedsSource $source, $entity, $target, array $values, array $mapping) {
  // Don't do anything if we weren't given any data.
  if (empty($values)) {
    return;
  }

  // The Feed API was changed so that the $values are passed as an array rather
  // than a single value. For backwards compatibility, support both.
  if (!is_array($values)) {
    $values = array($values);
  }
  $role = str_replace('role_prices_', '', $target);
  // Assign the value.
  $entity->role_prices[$role] = reset($values);
}
