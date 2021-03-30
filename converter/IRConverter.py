src = open("earplug_data.txt", "r")
dest = open("earplug.h", "w")

lines = src.readlines()

dest.write("t_float earplug_impulses[368][2][128] = {\n")
for line in lines:
    if line[0] == "*":
        continue
    if line == '\n':
        continue
    else:
        irs = line.split(' ')
        irs.pop()
        left = irs[:len(irs) // 2]
        right = irs[len(irs) // 2:]

        dest.write('{\n{')
        dest.write(', '.join(left))
        dest.write('},\n')

        dest.write('{')
        dest.write(', '.join(right))
        dest.write('}\n},\n')
dest.write('};\n')
src.close()
dest.close()
