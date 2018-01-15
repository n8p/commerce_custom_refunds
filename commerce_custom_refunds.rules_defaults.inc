<?php
/**
 * @file
 * commerce_custom_refunds.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_custom_refunds_default_rules_configuration() {
  $items = array();
  $items['rules_update_order_status_on_full_refund'] = entity_import('rules_config', '{ "rules_update_order_status_on_full_refund" : {
      "LABEL" : "Update order status on full refund",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Refunds" ],
      "REQUIRES" : [ "commerce_custom_refunds", "commerce_order" ],
      "ON" : { "commerce_custom_refunds_refund_transaction_is_processed" : [] },
      "IF" : [
        { "commerce_custom_refunds_order_balance_vs_total" : { "commerce_order" : [ "commerce_order" ] } }
      ],
      "DO" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "commerce_order" ], "order_status" : "refund" } }
      ]
    }
  }');
  $items['rules_update_order_status_on_partial_refund'] = entity_import('rules_config', '{ "rules_update_order_status_on_partial_refund" : {
      "LABEL" : "Update order status on partial refund",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Commerce Payment" ],
      "REQUIRES" : [ "commerce_custom_refunds", "commerce_order" ],
      "ON" : { "commerce_custom_refunds_refund_transaction_is_processed" : [] },
      "IF" : [
        { "NOT commerce_custom_refunds_order_balance_vs_total" : { "commerce_order" : [ "commerce_order" ] } }
      ],
      "DO" : [
        { "commerce_order_update_status" : {
            "commerce_order" : [ "commerce_order" ],
            "order_status" : "refund_partial"
          }
        }
      ]
    }
  }');
  return $items;
}
