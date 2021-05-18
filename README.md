# MatchC
C pattern matching library

## Current problems:
* repeating variable "constraints"
* variable function name matching
* general wildcard...? 
  * f[_] 
  * f[__] etc
* regular constraints
* f[x + y] doesn't match f[PLUS[x,y]] (Works w. variables in pattern)
* Remove :=
> * g[___] should match to g and g[] 
> * while g[_] shouldn't match to g