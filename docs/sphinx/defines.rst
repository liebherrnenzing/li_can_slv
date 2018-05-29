.. _lcs-defines:

===================
Build configuration
===================

The build can be configured via C preprocessor definitions using the header file :literal:`li_can_slv_config.h`.
The relevant defines therefore can be found in this section.

Mandatory Defines
-----------------

The defines in this subsection are obligated and have to be set correctly otherwise the stack could not compiled as expected.

Used Feature Set
````````````````
.. doxygengroup:: li_can_slv_defines_a
   :project: li_can_slv
   :content-only:

.. attention::
   One of the Feature Set must be used otherwise the stack would not work as expected.

Number of logical modules
`````````````````````````
.. doxygengroup:: li_can_slv_defines_b
   :project: li_can_slv
   :content-only:

.. _defines_mainmon:

Main/Monitor systems
--------------------
.. doxygengroup:: li_can_slv_defines_mainmon
   :project: li_can_slv
   :content-only:

.. _defines_reconnect:

CAN reconnect (Autobaud)
------------------------
.. doxygengroup:: li_can_slv_defines_reconnect
   :project: li_can_slv
   :content-only:

System alarms and system commands
---------------------------------
.. doxygengroup:: li_can_slv_defines_system
   :project: li_can_slv
   :content-only:

Synchronous data handling
-------------------------
.. doxygengroup:: li_can_slv_defines_synchrony
   :project: li_can_slv
   :content-only:

Asynchronous data handling
--------------------------
.. doxygengroup:: li_can_slv_defines_asynchrony
   :project: li_can_slv
   :content-only:

HW depended settings
--------------------
.. doxygengroup:: li_can_slv_defines_arch
   :project: li_can_slv
   :content-only:

Example
-------

This example shows how a :literal:`li_can_slv_config.h` file could look like.

.. literalinclude:: ./_static/li_can_slv_config.txt
   :language: c
   :linenos:
   
