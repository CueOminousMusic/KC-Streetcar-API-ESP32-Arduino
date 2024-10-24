prairielandelec (Mac) had  done some digging to figure out the undocumented API for KC's Streetcar here:
https://github.com/prairielandelec/KC-Streetcar-API

This project implements the concept in Arduino-ESP32.  
As of the initial commmit, several of the time constructs aren't yet implemented.  
The northbound/southbound feature is also not yet implemented, as it didn't seem to matter for my use case.  

The example runs on a Cheap Yellow Display (ESP32-2432S028R dev board), but I think the header would be usable on any ESP32.

Re-implementing this on a non-ESP board would need to replace WiFiClientSecure.h


| stopId | Stop Name                                            |
| -------| -------------                                        |
|  1601  |  RIVER MARKET WEST ON DELAWARE AT 4TH ST Southbound  |
|  1602  |  NORTH LOOP ON MAIN AT 7TH ST Southbound             |
|  1603  |  LIBRARY ON MAIN AT 9TH ST Southbound                |
|  1604  |  METRO CENTER ON MAIN AT 12TH ST Southbound          |
|  1605  |  POWER & LIGHT ON MAIN AT 14TH ST Southbound         |
|  1606  |  KAUFFMAN CENTER ON MAIN AT 16TH ST Southbound       |
|  1607  |  CROSSROADS ON MAIN AT 19TH ST Southbound            |
|  1608  |  UNION STATION ON MAIN AT PERSHING Southbound        |
|  1609  |  CROSSROADS ON MAIN AT 19TH ST Northbound            |
|  1610  |  KAUFFMAN CENTER ON MAIN AT 16TH ST Northbound       |
|  1611  |  POWER & LIGHT ON MAIN AT 14TH ST Northbound         |
|  1612  |  METRO CENTER ON MAIN AT 12TH ST Northbound          |
|  1613  |  LIBRARY ON MAIN AT 9TH ST Northbound                |
|  1614  |  NORTH LOOP ON MAIN AT 7TH ST Northbound             |
|  1615  |  CITY MARKET ON WALNUT AT 5TH ST Eastbound           |
