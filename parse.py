import re
import json
import logging

class TMLParser:
    def __init__(self, tml_input=None, file_path=None):
        self.tml_input = tml_input
        self.file_path = file_path
        self.logger = logging.getLogger(__name__)

        if file_path:
            self.load_from_file()

    def load_from_file(self):
        try:
            with open(self.file_path, 'r') as file:
                self.tml_input = file.read()
        except Exception as e:
            self.logger.error("Error loading TML file: %s", str(e))
            raise

    def parse(self):
        try:
            name_pattern = re.compile(r'<name>(.*?)<\/name>')
            description_pattern = re.compile(r'<description>(.*?)<\/description>')
            params_pattern = re.compile(r'<params>(.*?)<\/params>', re.DOTALL)

            name_match = name_pattern.search(self.tml_input)
            description_match = description_pattern.search(self.tml_input)
            params_match = params_pattern.search(self.tml_input)

            if not (name_match and description_match and params_match):
                raise ValueError("Invalid TML input format")

            name = name_match.group(1).strip()
            description = description_match.group(1).strip()
            params_text = params_match.group(1).strip()
            params = {}
            params_text = params_text.strip().split('\n')
            for param in params_text:
                param_parts = param.split(':')
                param_name = param_parts[0].strip()
                param_description = param_parts[1].strip()
                params[param_name] = param_description

            output = {
                "type": "function",
                "function": {
                    "name": name,
                    "description": description,
                    "parameters": {
                        "type": "object",
                        "properties": {param: {"type": "string", "description": desc} for param, desc in params.items()},
                        "required": list(params.keys())
                    }
                }
            }

            return output
        except Exception as e:
            self.logger.error("An error occurred while parsing TML input: %s", str(e))
            raise