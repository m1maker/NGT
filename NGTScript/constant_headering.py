
import re

with open("input.txt", "r") as f_in, open("output.txt", "w") as f_out:
    for line in f_in:
        match = re.search(r"const int\s+(\w+)\s+=\s+(SDLK_\w+);", line)

        if match:
            name = match.group(1)
            code = match.group(2)
            prop = f'engine->RegisterGlobalProperty("const int {code}", (void*)&{name});\n'
            f_out.write(prop)
        else:
            f_out.write(line)


