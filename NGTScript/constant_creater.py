import re
i=0
with open("input.txt", "r") as f_in, open("output.txt", "w") as f_out:
    for line in f_in:
        match = re.search(r"SDLK_\w+", line)

        if match:
            code = match.group()
            const = f"const int kkke{i} = {code};\n"
            f_out.write(const)
            i+=1
        else:
            f_out.write(line)
