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
    - `create` : used to create or add files to a corpus
        - `--create NAME` : create a named corpus with the result of the query
        - `--append ID` : append the result to the corpus specified by the ID
        - `--number NBR` : needs to be at the front, number of results per page (default: 0 = unlimited)
        - `--page NBR` : works in conjuction with number, to paginate the result
        - `--order ORDER` : specifies the return order (API asc/desc or size asc/desc)
        - `--type TYPE` : needs to be after the page, number and order (if specified), can appear multiple times (text, image)
        - `--source NAME` : specifies a request on this source name, can also appear many times
        - `--PARAM_NAME VALUE` : specifies a parameter on the nearest source on the left, can also appear many times
        - `--op OP` : specifies the comparison operator (=, !=, <, >, <=, >=) on a previous parameter. If none is used, = will be used

## Files
- `files`
    - `--id ID` : displays informations about the file with the given ID
    - `list` : used to build a complex query
        - `--number NBR` : needs to be at the front, number of results per page (default: 0 = unlimited)
        - `--page NBR` : works in conjuction with number, to paginate the result (default: 0 = the first page)
        - `--order ORDER` : specifies the return order (API asc/desc or size asc/desc)
        - `--type TYPE` : needs to be after the page, number and order (if specified), can appear multiple times (text, image)
        - `--source NAME` : specifies a request on this source name, can also appear many times
        - `--PARAM_NAME VALUE` : specifies a parameter on the nearest source on the left, can also appear many times
        - `--op OP` : specifies the comparison operator (=, !=, <, >, <=, >=) on a previous parameter. If none is used, = will be used

## Build commands example:

### DB
These commands work with both the `files list` and `corpus create`.

- `harvester files list --number 150 --page 2 --type image` : will get all files from image APIs in the DB starting from the 301st to the 450th
- `harvester files list --number 50 --order size_desc` : will get the 50 heaviest files in the DB
- `harvester files list --source Twitter --retweets 50 --op ">" --source TMDB_Poster` : retrieves all tweets with more than 50 retweets and all posters from TMDB in default order
- `harvester files list --type image --source Twitter --retweets 50 --op ">" --source TMDB_Poster` : same as above, but will only retrieve the posters
- `harvester files list --number 1000 --source Twitter` : retrieves the 1000 first tweets in the DB

## Settings
- `logger` : displays the (2 or 3, output path is not displayed if the output is stdout) logger settings
    - `--setting level --value VALUE` : sets the logger level to one of these: debug, info, warning, error, none
    - `--setting output --value VALUE` : sets the logger output to one of these: stdout, file
    - `--setting output_path --value VALUE` : sets the logger output path to the given value
    - `--clear` : deletes the logfile if existing
- `storage` : displays the storage root
    - `--migrate ABSOLUTE_PATH` : migrates the storage to this absolute path
