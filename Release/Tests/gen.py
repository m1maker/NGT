import os
import re

def parse_angel_script(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    # Regular expressions to match different constructs
    namespace_pattern = r'namespaces+"([^"]+)"'
    enum_pattern = r'enums+(w+)'
    enumval_pattern = r'enumvals+(w+)s+(w+)s+(d+)'
    objtype_pattern = r'objtypes+"([^"]+)"s+(d+)'
    typedef_pattern = r'typedefs+(w+)s+"([^"]+)"'
    funcdef_pattern = r'funcdefs+"([^"]+)"'
    objbeh_pattern = r'objbehs+"([^"]+)"s+d+s+"([^"]+)"'
    objmthd_pattern = r'objmthds+"([^"]+)"s+"([^"]+)"'

    namespaces = re.findall(namespace_pattern, content)
    enums = re.findall(enum_pattern, content)
    enumvals = re.findall(enumval_pattern, content)
    objtypes = re.findall(objtype_pattern, content)
    typedefs = re.findall(typedef_pattern, content)
    funcdefs = re.findall(funcdef_pattern, content)
    objbehs = re.findall(objbeh_pattern, content)
    objmthds = re.findall(objmthd_pattern, content)

    return {
        'namespaces': namespaces,
        'enums': enums,
        'enumvals': enumvals,
        'objtypes': objtypes,
        'typedefs': typedefs,
        'funcdefs': funcdefs,
        'objbehs': objbehs,
        'objmthds': objmthds,
    }

def generate_documentation(parsed_data):
    documentation = []

    documentation.append("# AngelScript Documentationn")

    # Namespaces
    documentation.append("## Namespacesn")
    for ns in parsed_data['namespaces']:
        documentation.append(f"- **Namespace:** {ns}n")

    # Enums
    documentation.append("n## Enumsn")
    for enum in parsed_data['enums']:
        documentation.append(f"- **Enum:** {enum}n")
        for enumval in parsed_data['enumvals']:
            if enumval[0] == enum:
                documentation.append(f"  - **Value:** {enumval[1]} (Code: {enumval[2]})n")

    # Objtypes
    documentation.append("n## Objtypesn")
    for objtype in parsed_data['objtypes']:
        documentation.append(f"- **Objtype:** {objtype[0]} (ID: {objtype[1]})n")

    # Typedefs
    documentation.append("n## Typedefsn")
    for typedef in parsed_data['typedefs']:
        documentation.append(f"- **Typedef:** {typedef[1]} as {typedef[0]}n")

    # Function Definitions
    documentation.append("n## Function Definitionsn")
    for funcdef in parsed_data['funcdefs']:
        documentation.append(f"- **Function Definition:** {funcdef}n")

    # Object Behaviors
    documentation.append("n## Object Behaviorsn")
    for objbeh in parsed_data['objbehs']:
        documentation.append(f"- **Object Behavior:** {objbeh[0]} - {objbeh[1]}n")

    # Object Methods
    documentation.append("n## Object Methodsn")
    for objmthd in parsed_data['objmthds']:
        documentation.append(f"- **Object Method:** {objmthd[1]} of {objmthd[0]}n")

    return ''.join(documentation)

def main():
    all_data = {
        'namespaces': [],
        'enums': [],
        'enumvals': [],
        'objtypes': [],
        'typedefs': [],
        'funcdefs': [],
        'objbehs': [],
        'objmthds': [],
    }
    file_path = input("Enter path to the dumped configuration file");
    parsed_data = parse_angel_script(file_path)

    for key in all_data:
                all_data[key].extend(parsed_data[key])

    documentation = generate_documentation(all_data)

    with open('angel_script_documentation.md', 'w') as doc_file:
        doc_file.write(documentation)

    print("Documentation generated: angel_script_documentation.md")

if __name__ == "__main__":
    main()