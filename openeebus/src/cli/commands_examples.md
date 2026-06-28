# Commands examples

## CS LPC
> cs_lpc set power_limit 3500.5 false true  
> cs_lpc get power_limit  
> cs_lpc set failsafe_limit 3500.5 true  
> cs_lpc get failsafe_limit  
> cs_lpc set failsafe_duration PT3H02M3S true  
> cs_lpc get failsafe_duration  
> cs_lpc start heartbeat  
> cs_lpc stop heartbeat  

## MU MPC

MU MPC commands format:

> mu_mpc get <measurement_name>
> mu_mpc set <measurement_name> <value>

The list of available measurement names:
* power_total
* power_phase_a
* power_phase_b
* power_phase_c
* energy_consumed
* energy_produced
* current_phase_a
* current_phase_b
* current_phase_c
* voltage_phase_a
* voltage_phase_b
* voltage_phase_c
* voltage_phase_ab
* voltage_phase_bc
* voltage_phase_ac
* frequency

For power_total use e.g.:

> mu_mpc get power_total  
> mu_mpc set power_total 1200.0  

