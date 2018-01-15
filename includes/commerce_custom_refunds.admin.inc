<?php
/**
 * @file
 * Admin functions for Commerce Custom Refunds module.
 */

/**
 * Form builder function for module settings.
 *
 * @return
 *   Drupal form for admin settings.
 */
function commerce_custom_refunds_settings_form() {
  $form = array();
  $options = array();

  $form['refund_store_credit_discount'] = array(
    '#type' => 'select',
    '#title' => t('Store credit discount'),
    '#default_value' => variable_get('refund_store_credit_discount', 'discount_store_credit'),
    '#description' => t('Select the discount type to be used as the basis for generating store credit coupons.'),
    '#options' => commerce_discount_entity_list(),
    '#required' => TRUE,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Submit handler: saves the settings.
 */
function commerce_custom_refunds_settings_form_submit($form, &$form_state) {
  // Exclude unnecessary elements.
  form_state_values_clean($form_state);

  foreach ($form_state['values'] as $key => $value) {
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    variable_set($key, $value);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * This overrides the default payments view tab with a different view.
 *
 * @see commerce_payment_ui_order_tab().
 */
function commerce_custom_refunds_payment_ui_order_tab($order) {
  // Display the payments View.
  return commerce_embed_view('commerce_order_payments_refunds', 'defaults', array($order->order_id));
}

/**
 * Form callback: allows the user to perform a refund on a credit account transaction.
 *
 * @param array $form
 *   Nested array of form elements that comprise the form.
 * @param array $form_state
 *   Nested array of the stored state of the form values.
 * @param $order
 *   The order the transaction is on.
 * @param $transaction
 *   The payment transaction object to be refunded.
 *
 * @return $form
 *   Returns the refund form.
 */
function commerce_custom_refunds_credit_accounts_refund_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  // Get max refund amount available based on other refunds.
  $max_refund_amount = $transaction->amount;
  if (!empty($transaction->data['refunds'])) {
    foreach ($transaction->data['refunds'] as $refund_transaction_id => $refund_transaction_amount) {
      $max_refund_amount += $refund_transaction_amount;
    }
  }
  $form_state['max_refund_amount'] = $max_refund_amount;

  $form = commerce_custom_refunds_refund_form_items($form, $transaction, $order);

  // Remove store credit option, since we don't want that availble for credit
  // account refunds.
  unset($form['payment']);

  // Pass an identifier to the submit function so we know to process
  // this as a credit account refund transaction.
  $form['payment']['payment_method'] = array(
    '#type' => 'hidden',
    '#value' => 'accounts',
  );

  // Override and replace the existing form validation.
  $form['#validate'] = array('commerce_custom_refunds_refund_form_validate');
  $form['#submit'] = array('commerce_custom_refunds_refund_form_submit');

  $form = confirm_form($form, t('Are you sure you want to issue a refund?'), 'admin/commerce/orders/' . $order->order_id . '/payment', '', t('Refund'), t('Cancel'), 'confirm'
  );

  // Prevents multiple submissions of the refund form.
  $form['actions']['submit']['#attributes'] = array(
    'onclick' => commerce_custom_refunds_form_onclick_js_disable(),
  );

  return $form;
}
