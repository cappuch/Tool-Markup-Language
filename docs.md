**Introduction:**
Tool Markup Language (TML) is a lightweight markup language designed for describing tools, functions, and their parameters in a structured format. It provides a simple yet effective way to define tools and their usage, making it easy for language models to understand and process. The syntax is made to be easy to learn, and easy to explain to an LLM, to make a self-sufficient LLM.

**Syntax:**
TML syntax follows a straightforward structure consisting of specific tags to define the name, description, and parameters of a tool. It is heavily inspired by XML or HTML syntax. Here's a breakdown of the syntax:

- `<name>`: This tag is used to specify the name of the tool or function. It should be followed by the actual name enclosed within `<name>` and `</name>` tags.

- `<description>`: This tag is used to provide a brief description of the tool or function's purpose and functionality. It should be followed by the description text enclosed within `<description>` and `</description>` tags.

- `<params>`: This tag is used to define the parameters required by the tool or function. Each parameter should be listed with its name and description, separated by a colon (`:`). Multiple parameters should be listed on separate lines within the `<params>` and `</params>` tags.

**Example:**
Here's an example of how a tool is described using TML syntax:

```tml
<name>calculate_distance</name>
<description>Calculate the distance between two points in a coordinate system</description>
<params>
x1: The x-coordinate of the first point
y1: The y-coordinate of the first point
x2: The x-coordinate of the second point
y2: The y-coordinate of the second point
</params>
```

**Usage:**
TML is intended to be used as a means of providing language models with structured information about tools and functions. It allows for easy parsing and interpretation of tool descriptions, enabling models to understand the purpose and requirements of various tools.

**Python Examples:**

```py
from TMLParser import TMLParser
parser = TMLParser(file_path='/google_search.tml') # file loading
parser.parse()
```

```py
from TMLParser import TMLParser
parser = TMLParser(tml_input="") # string loading
parser.parse()
```

TMLParser.TML will always return an equivalent dictionary. If using the C version, you must compile it, but then you can use it with any other language if there isn't a native one for your one. Feel free to make PRs with other languages, and YMMV.
