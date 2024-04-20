const fs = require('fs');

class TMLParser {
    constructor(tmlInput = null, filePath = null) {
        this.tmlInput = tmlInput;
        this.filePath = filePath;
        this.logger = require('log4js').getLogger();

        if (filePath) {
            this.loadFromFile();
        }
    }

    loadFromFile() {
        try {
            this.tmlInput = fs.readFileSync(this.filePath, 'utf-8');
        } catch (error) {
            this.logger.error(`Error loading TML file: ${error}`);
            throw error;
        }
    }

    parse() {
        try {
            const namePattern = /<name>(.*?)<\/name>/;
            const descriptionPattern = /<description>(.*?)<\/description>/;
            const paramsPattern = /<params>(.*?)<\/params>/s;

            const nameMatch = this.tmlInput.match(namePattern);
            const descriptionMatch = this.tmlInput.match(descriptionPattern);
            const paramsMatch = this.tmlInput.match(paramsPattern);

            if (!(nameMatch && descriptionMatch && paramsMatch)) {
                throw new Error("Invalid TML input format");
            }

            const name = nameMatch[1].trim();
            const description = descriptionMatch[1].trim();
            const paramsText = paramsMatch[1].trim().split('\n');
            const params = {};
            for (const param of paramsText) {
                const paramParts = param.split(':');
                const paramName = paramParts[0].trim();
                const paramDescription = paramParts[1].trim();
                params[paramName] = paramDescription;
            }

            const output = {
                type: "function",
                function: {
                    name: name,
                    description: description,
                    parameters: {
                        type: "object",
                        properties: Object.fromEntries(Object.entries(params).map(([param, desc]) => [param, { type: "string", description: desc }])),
                        required: Object.keys(params)
                    }
                }
            };

            return output;
        } catch (error) {
            this.logger.error(`An error occurred while parsing TML input: ${error}`);
            throw error;
        }
    }
}

module.exports = TMLParser;
