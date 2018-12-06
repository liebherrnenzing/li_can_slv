.. _lcs-api:

*************
API Reference
*************

The Liebherr CAN Slave API consist of differnet functions organaized by functionality.


Core
====

Functions
---------

Init, Start and State
`````````````````````

.. doxygenfunction:: lcsa_init(can_config_bdr_t baudrate)

.. doxygenfunction:: lcsa_start(void)

.. doxygenfunction:: lcsa_set_state

.. doxygenfunction:: lcsa_get_state

Mandatory Function
``````````````````

.. doxygendefine:: lcsa_trigger_5msec

Configuration
`````````````

.. doxygenfunction:: lcsa_add_module

.. doxygendefine:: lcsa_set_module_critical

.. doxygendefine:: lcsa_is_module_valid

.. doxygendefine:: lcsa_get_main_module_number

.. doxygendefine:: lcsa_set_baudrate

.. doxygenfunction:: lcsa_get_baudrate


Enums
-----

.. doxygenenum:: lcsa_state_t
.. doxygenenum:: lcsa_module_change_voter_state_t


Types
-----

.. doxygengroup:: li_can_slv_api_types
   :project: li_can_slv


Defines
-------

Configuration
`````````````

.. doxygengroup:: li_can_slv_api_defines_default_module_nr
   :project: li_can_slv
   :content-only:

.. doxygengroup:: li_can_slv_api_defines_config
   :project: li_can_slv
   :content-only:

Baud Rate
`````````

.. doxygengroup:: li_can_slv_api_defines_baudrate
   :project: li_can_slv
   :content-only:


Synchronous Protocol Handling
=============================

Functions
---------

.. doxygengroup:: li_can_slv_api_sync
   :project: li_can_slv
   :content-only:

Set Callback Functions
----------------------

.. doxygengroup:: li_can_slv_api_sync_cbk
   :project: li_can_slv
   :content-only:


Download / Upload
=================

Set Handle Functions
--------------------

.. doxygengroup:: li_can_slv_api_dload
   :project: li_can_slv
   :content-only:

.. doxygengroup:: li_can_slv_api_uload
   :project: li_can_slv
   :content-only:

Error and Message
=================

Functions
---------

.. doxygengroup:: li_can_slv_api_sync_error
   :project: li_can_slv
   :content-only:

Defines
-------

Error Code
``````````

.. doxygengroup:: li_can_slv_api_error_code_defines
   :project: li_can_slv
   :content-only:

Priority
````````

.. doxygengroup:: li_can_slv_api_error_msg_prio
   :project: li_can_slv
   :content-only:

Group
`````

.. doxygengroup:: li_can_slv_api_error_msg_group
   :project: li_can_slv
   :content-only:

SubGroup
````````

.. doxygengroup:: li_can_slv_api_error_msg_subgroup
   :project: li_can_slv
   :content-only:

Types
-----

.. doxygengroup:: li_can_slv_api_types_error
   :project: li_can_slv
   :content-only:


.. _api_reconnect:

CAN reconnect (Autobaud)
========================

Set Callback Functions
----------------------

.. doxygengroup:: li_can_slv_api_reconnect_online
   :project: li_can_slv

.. doxygengroup:: li_can_slv_api_reconnect_startup
   :project: li_can_slv

Change Module Number Handling
=============================

.. doxygengroup:: lcsa_api_change_module_nr
   :project: li_can_slv
