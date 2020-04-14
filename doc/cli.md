# Command Line Interface

harvester apis list
harvester apis list "nom_de_l'_api"
harvester apis list "nom_de_l_api" db
harvester apis list "nom_de_l_api" web
harvester corpus list
harvester corpus id "id"
harvester corpus search "texte à chercher"
harvester files list (--page en option)
harvester files id "id"

This file's goal is to document all command line commands available. All commands are to be appended to the executable file.

## General operation
When running a command, you should be able to use `-h` to ask for help at any stage. It will display what options are available to you. The help is also displayed when you entered an unsupported command.

```sh
harvester -h

Usage: bin/cli [subcommand]
Description: Corpus Harvester

Subcommand(s):
-h: displays help
apis: Api function.
corpus: Corpus function.
files: Files function.
```

## apis
Every operation relating to API informations, modifications...

### list
List every APIs available.

*ex.:* `harvester apis list`

**Params:**
* `--db` displays parameters to use against locally saved files
* `--web` displays parameters to use against web queries
* `--name`
