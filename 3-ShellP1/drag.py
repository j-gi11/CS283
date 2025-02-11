def encode_text_file(filename):
    def group_runs(s):
        if not s:
            return []
        groups = []
        current_char = s[0]
        count = 1
        for c in s[1:]:
            if c == current_char:
                count += 1
            else:
                groups.append((current_char, count))
                current_char = c
                count = 1
        groups.append((current_char, count))
        return groups

    with open(filename, 'r') as f:
        lines = f.readlines()

    print("const unsigned char dragon_compressed[] = {")
    for line in lines:
        runs = group_runs(line)
        elements = []
        for char, count in runs:
            elements.append(repr(char))
            elements.append(str(count))
        line_str = '    ' + ', '.join(elements) + ','
        print(line_str)
    print("};")


encode_text_file("dragon.txt")