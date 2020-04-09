# API JSON documentation

This documentation's goal is to explain how to make your own API parser in the JSON format, to include them in the Harvester.

If you wish, the JSON schema which validates and describes the structure to write is in data/api_schema.json. Here we will explain with examples and concrete cases how to write an API parser.

## Example JSON with all available parameters

```json
{
    "name": "My_API",
    "url": "http://myapi.endpoint/v1/etc/",
    "method": "GET",
    "api_type": "image",
    "truncate_before": 2,
    "truncate_after": 3,
    "path_to_results": ["response", "list_of_elements"],
    "response_main_item": "my_image",

    "request":[

        {
            "type": "int",
            "api_name": "api_parameter_1",
            "name": "api_parameter_in_db_1",
            "required": true,
            "position": "body",
            "relevant": true,
            "default_value": "cats"
        },
        {
            "type": "string",
            "api_name": "api_parameter_2",
            "required": false,
            "position": "body",
            "relevant": true,
            "values": ["possible_value_1", "possible_value_2"]
        },
        {
            "type": "string",
            "api_name": "login_key",
            "required": true,
            "position": "header",
            "relevant": false
        }


    ],

    "response":[

        {
            "type": "string",
            "api_name": "api_response_1",
            "name": "api_response_in_db_1",
            "relevant": true,
        },
        {
            "type": "string",
            "api_name": "images_base_url",
            "relevant": false,
        },
        {
            "type": "image_link",
            "api_name": "api_picture",
            "name": "db_picture",
            "relevant": true,
            "string_appends": [
                {
                    "is_parameter_name": false,
                    "value": "http://"
                },
                {
                    "is_parameter_name": true,
                    "value": "images_base_url"
                }
            ],
            "string_prepends": [
                {
                    "is_parameter_name": false,
                    "value": ".jpg"
                }
            ]
        }

    ]
}
```

This example contains every parameters supported by the API parser, and details about every one of them is just below.

## JSON root

These parameters are the descriptors of the API and the parameters for a successful call.

### name
**required**

Name of the API, used by the Harvester and users to select an API. It must be unique amongst the settings. Only ASCII letters, digits or underscores are accepted.

*ex.:* `"Twitter"`

### url
**required**

API endpoint to call when doing a query.

*ex.:* `"https://api.themoviedb.org/3/search/movie"`

### method
**required**

HTTP method to use when querying the endpoint.

*possible values:* `"GET"`, `"POST"`

### api_type
**required**

Describes the type of the mail element retrieved from the API, an image, a text...

*possible values:* `"text"`, `"image"`

### truncate_before
Removes `n` characters at the front of the query response before parsing it. Extremely edge case used by APIs like Flickr as it sends back an invalid JSON and we need to remove the 14 first and the latest character to have a valid JSON to parse.

*ex.:* `14`

### truncate_after
Similar to `truncate_before` but at the end of the response.

*ex.:* `1`

### response_main_item
**required**

Response parameter name to treat as the main value (the `text` or `image`) and to save it as a file and not a metadata.

*ex.:* `"picture"`

### path_to_results
**required**

JSON path of child to reach the response array in the full response. See the example below.

```json
{
    "response_time": "34ms",
    "date": "2020-04-01",
    "responses_wrapper": {
        "real_responses": [
            {
                "id": 1,
                "name": "John"
            },
            {
                "id": 2,
                "name": "James"
            },
            {
                "id": 3,
                "name": "Joan"
            }
        ]
    }
}
```

*ex.:* `["responses_wrapper", "real_responses"]`

### request
**required**

List of all parameters that can or must be sent to the API for a query. See full details below.

### response
**required**

List of all response parameters to save (or not) for each element of the response. See full details below.

## Request parameters

The `request` object in the JSON is a list of different request objects, as described here.

### type
**required**

Type of the parameter to use.

*possible values:* `"string"`, `"int"`

### api_name
**required**

Name of the parameter on the API side.

*ex.:* `"page_count"`

### name

Name of the parameter on the Harvester side. If not provided, `api_name` is used. The user and Harvester will only manipulate this name.

*ex.:* `"page"`

### required
**required**

Boolean to tell if the parameter is required to send a query.

*ex.:* `true`

### position
**required**

Place to put the parameter in the HTTP query.

*possible values:* `"body"`, `"header"`

### relevant
**required**

Boolean to save the parameter value as a metadata.

*ex.:* `false`

### default_value

If no value is provided in a query and this value is set, will use this one. Must be represented as a string.

*ex.:* `"1"` for a page number for instance

### values

Array of possible values for this parameter. If a value is given by the user or by a default value, it will check it against this array before doing (or not) the query. Values must be strings.

*ex.:* `["recent", "popular", "mixed"]`

## Response parameters

The `response` object in the JSON is a list of different response objects, as described here.

### type
**required**

Same as the `request.type`.

### api_name
**required**

Same as the `request.api_name`

### name

Same as the `request.name`

### relevant
**required**

Same as the `request.relevant`

### string_prepends

This adds a list of fixed or API values to the parameter response string before treating it. Useful when fetching images for instance where only the image ID is given and you must prepend `"http://"` or a farm ID given in the response. Values are prepended left to right, first to last in array. It is an array of objects as described below:

#### is_parameter_name
**required**

Boolean to `true` if the `value` parameter is the name of an API request or response parameter (must be described in the JSON, you can put it to non relevant if you wish to not save it). Putting it to `false` will literally use the `value` string.

#### value
**required**

Name of an API parameter or a string to add, based on the `is_parameter_name` boolean.

```json
"string_prepends": [
    {
        "is_parameter_name": false,
        "value": "http://"
    },
    {
        "is_parameter_name": true,
        "value": "api_farm_url"
    }
]
```

If the response parameter value is `/image.jpg` and the value of `api_farm_url` is `mywebsite.com`, the output which will be parsed is `http://mywebsite.com/image.jpg`

### string_appends

Same as `string_prepends` but to append value at the end of the response. First in array will be the first value appended, and so on.

## Typical and magic names to use

### _api_id

Using the `_api_id` name will check if another entry of the same ID and same API name is already in the database and will skip it if it is.

### _page

Attribute representing a number which can be incremented to retrieve more results. Please set the default value to the minimum page compatible with the API for better automation.

### _page_count

Number of elements in a page to retrieve.

### query

Name to use to represent a string query in a request (or response).

### language

Parameter which represents a language. No parsing is done on it, so you can get `english` or `en` values.

### date

Parameter to describe what date the element was created. No parsing is done on it for now.

## Note on sensitive values

Values such as API keys shouldn't have to be filled by the user at each query, which is a tedious process. So when loading an API from its descriptor file such as `/path/api.json`, the Harvester will look for an env file (which is gitignored) as `/path/api.env.json` which fills in default values.

```json
{
    "api_key": "MY_API_KEY",
    "other_default_to_override": "my_value
}
```

These values will be loaded at the same time than the API, and overrides other default values if present in the descriptor.
