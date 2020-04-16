# CorpusHarvester - Client Line Interface

In order to have access to our software, the basic method is to use :
`bin/cli`

### Parameters

- `apis` : Api function.
    - `list` : Api list 
    - `--name ""` : Specify a api
        - `--db` : Show specific parameter for the database.
        - `--web` : Show specific parameter for the web.

- `corpus` : Corpus function. 
    - `create`̀ Allow us to create a corpus with a given `--name`.
        - `--n_element ""` : Number of element in a corpus.
        - `--image` : Specify we want image.
        - `--video` : Specify we want video.
        - `--text` : Specify we want text.
        - `--source ""` : Specify the source we want to use.

        Example : `./bin/cli corpus create --name "star wars"` 
        Here, we are create a corpus with the name star wars.

    - `list` : List all the available corpus.
        - `--name ""` : Get a corpus by his name.
  
        Example : `./bin/cli corpus list`
        Example : `./bin/cli corpus list --name "star wars"`
        
    - `--id ""` : Get a corpus by his id.
        
- `files` : Files function.
    - `list` : List of the files.
    - `--id ""` : Get from his id.
    