use std::fs::File;
use std::io::Read;
use regex::Regex;
use serde_json::{json, Value};
use log::{error, info};

struct TMLParser {
    tml_input: Option<String>,
    file_path: Option<String>,
}

impl TMLParser {
    fn new(tml_input: Option<String>, file_path: Option<String>) -> Self {
        Self { tml_input, file_path }
    }

    fn load_from_file(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        if let Some(file_path) = &self.file_path {
            let mut file = File::open(file_path)?;
            let mut contents = String::new();
            file.read_to_string(&mut contents)?;
            self.tml_input = Some(contents);
        }
        Ok(())
    }

    fn parse(&self) -> Result<Value, Box<dyn std::error::Error>> {
        let tml_input = self.tml_input.as_ref().ok_or("TML input not found")?;

        let name_pattern = Regex::new(r"<name>(.*?)<\/name>").unwrap();
        let description_pattern = Regex::new(r"<description>(.*?)<\/description>").unwrap();
        let params_pattern = Regex::new(r"<params>(.*?)<\/params>").unwrap();

        let name = name_pattern.captures(tml_input).ok_or("Name not found")?[1].trim().to_string();
        let description = description_pattern.captures(tml_input).ok_or("Description not found")?[1].trim().to_string();
        let params_text = params_pattern.captures(tml_input).ok_or("Params not found")?[1].trim().to_string();

        let mut params = serde_json::Map::new();
        for param in params_text.lines() {
            let param_parts: Vec<&str> = param.split(':').map(|s| s.trim()).collect();
            if param_parts.len() == 2 {
                params.insert(param_parts[0].to_string(), json!({"type": "string", "description": param_parts[1]}));
            }
        }

        let output = json!({
            "type": "function",
            "function": {
                "name": name,
                "description": description,
                "parameters": {
                    "type": "object",
                    "properties": params,
                    "required": params.keys().cloned().collect::<Vec<_>>()
                }
            }
        });

        Ok(output)
    }
}