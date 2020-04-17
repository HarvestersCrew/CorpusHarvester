# Command Line Interface

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

## APIs
- `apis` :
    - `--name NAME` : display API information based on its name
        - `--db` : list the API DB parameters
        - `--web` : list the API web parameters
    - `list` : list available API names

## Corpuses
- `corpus` :
    - `--id ID` : displays informations about the corpus with the given ID
    - `list` : list all corpuses in default order (most recent to older)
        - `--name "TITLE"` : search all corpuses matching at least partially with this title
        - `--order ORDER` : display in the specified order (name asc/desc or date asc/desc), values to pass to be defined
    - `create` : to be defined
        - `--name "NAME"` : specify the name of the new corpus.

## Files
- `files`
    - `list` : list all files in DB, following some criteria
        - `--page` : page to use, starts at 0 (**required**)
        - `--number` : number of results per page (default: 100)
        - `--api API` : files only from this API
        - `--type TYPE` : type of files to find
        - `--order ORDER` : display in the specified order (name asc/desc or date asc/desc), values to pass to be defined
    - `--id ID` : displays informations about the file with the given ID

## Settings
- `logger` : displays the (2 or 3, output path is not displayed if the output is stdout) logger settings
    `--setting level --value VALUE` : sets the logger level to one of these: debug, info, warning, error, none
    `--setting output --value VALUE` : sets the logger output to one of these: stdout, file
    `--setting output_path --value VALUE` : sets the logger output path to the given value
- `storage` : displays the storage root
    `--migrate ABSOLUTE_PATH` : migrates the storage to this absolute path
