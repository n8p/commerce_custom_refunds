<?php
/**
 * @file
 * commerce_custom_refunds.rules.inc
 */

/**
 * Implements hook_rules_event_info().
 */
function commerce_custom_refunds_rules_event_info() {
  // So that we can use the entity_rules_events_variables() helper function.
  module_load_include('inc', 'entity', 'entity.rules');
  $events = array();

  $variables = array_merge(
    entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order')), TRUE, TRUE),
    entity_rules_events_variables('commerce_payment_transaction', t('Last completed transaction'), TRUE)
  );

  $events['commerce_custom_refunds_refund_transaction_is_processed'] = array(
    'label' => t('When a refund is applied to an order'),
    'group' => t('Commerce Custom Refunds'),
    'variables' => $variables,
  );

  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function commerce_custom_refunds_rules_condition_info() {
  $conditions = array();

  // Compare order balance with order total to determine refund state.
  $conditions['commerce_custom_refunds_order_balance_vs_total'] = array(
    'label' => t('Check if order is fully or partially refunded'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'wrapped' => TRUE,
      ),
    ),
    'group' => t('Commerce Custom Refunds'),
  );

  return $conditions;
}
