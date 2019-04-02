# Readme for acceptance tests

## Requirements

### shunit2

Getestet mit Fork des [Original-shunit2-Projekts] (https://github.com/kward/shunit2):

[shunit-with-reports] (https://bitbucket.org/eddiewebb/shunit-with-reports)

Das Original-Projekt wird aktuell relativ wenig weiterentwickelt und bietet
keine Report-Generierung im JUnit-Format, die für eine Buildserver-Integration
wichtig ist.  Der Fork bietet eine solche Generierung und integriert darüber
hinaus Bugfixes, die (noch) nicht den Weg in die Mainline gefunden haben.

Das shunit2-Skript aus dem Projekt muss im gleichen Verzeichnis liegen wie die
Testskripte.

#### Notwendige Anpassungen
TBD



