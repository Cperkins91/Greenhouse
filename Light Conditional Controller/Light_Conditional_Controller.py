1: import os
  2: import sys
  3: sys.path.append(os.path.abspath('/var/mycodo-root'))
  4: from mycodo.controllers.base_conditional import AbstractConditional
  5: from mycodo.mycodo_client import DaemonControl
  6: control = DaemonControl(pyro_timeout=30.0)
  7:
  8: class ConditionalRun(AbstractConditional):
  9:     def __init__(self, logger, function_id, message):
 10:         super(ConditionalRun, self).__init__(logger, function_id, message, timeout=30.0)
 11:
 12:         self.logger = logger
 13:         self.function_id = function_id
 14:         self.variables = {}
 15:         self.message = message
 16:         self.running = True
 17:
 18:     def conditional_code_run(self):
 19:         # Example code for learning how to use a Conditional. See the manual for more information.
 20:
 21:         self.logger.info("This INFO log entry will appear in the Daemon Log")
 22:         self.logger.error("This ERROR log entry will appear in the Daemon Log")
 23:
 24:         if not hasattr(self, "loop_count"):  # Initialize objects saved across executions
 25:             self.loop_count = 1
 26:         else:
 27:             self.loop_count += 1
 28:
 29:         # Replace "asdf1234" with a Condition ID
 30:         measurement = self.condition("2bfe72f2-a45b-452f-932a-139f71e0cf08") #Function 02 Avg (Past, Single) CH0 (Lux_Avg(Past,Single)) Light (lx) 2bfe72f2-a45b-452f-932a-139f71e0cf08
 31:         self.logger.info("Check this measurement in the Daemon Log. The value is {val}".format(val=measurement))
 32:
 33:         if measurement is not None:  # If a measurement exists
 34:             #self.message += "This message appears in email alerts and notes.\n"
 35:
 36:             if measurement < 800:  # If the measurement is less than 60 lux
 37:                 self.run_action("ae07dc65-74c9-43b1-8e4b-406ac3fc52fc", value = "ON") #MQTT Publish value #{"Lights": "ON"}
 38:                 self.run_action("d60d9bca-d9d5-4ff5-8b35-b1aa33f7b763")
 39:
 40:             elif measurement > 1000:  # Else If the measurement is greater than 100 lux
 41:                 self.run_action("ae07dc65-74c9-43b1-8e4b-406ac3fc52fc", value = "OFF" ) #{"Lights": "OFF"}
 42:                 self.run_action("05a25863-d6ca-4f9e-a448-df3bba9e786d")
 43:
 44:     def function_status(self):
 45:         # Example code to provide a return status for other controllers and widgets.
 46:         status_dict = {
 47:             'string_status': "The controller has looped {} times".format(self.loop_count),
 48:             'loop_count': self.loop_count,
 49:             'error': []
 50:         }
 51:         return status_dict
