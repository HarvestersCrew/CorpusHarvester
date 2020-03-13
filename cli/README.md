# CorpusHarvester - Client Line Interface

In order to have access to our software, the basic method is to use :
`bin/cli`

### Parameters

- `corpus` : All the command for corpus    - 
  - `create`̀ Allow us to create a corpus with a given `--name`.

  Example : `./bin/cli corpus create --name "star wars""` 

  Here, we are create a corpus with the name star wars.

  - `list` : List all the available corpus.
  
  Example : `./bin/cli corpus list`
  
    - Possibility to had some filter like the name, with the option `--name`. 
    
    Example : `./bin/cli corpus list --name "star wars""`
    