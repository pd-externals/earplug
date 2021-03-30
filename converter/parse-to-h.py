src = open("../earplug_data_src.txt", "r")
dest = open("../earplug~.h", "w")

lines = src.readlines()

dest.write("#include \"m_pd.h\"\n")
dest.write("t_float earplug_impulses[368][2][128] = {\n")
for line in lines:
    if line[0] == "*":
        continue
    if line == '\n':
        continue
    else:
        irs = line.split(' ')
        irs.pop()

        count = 0
        leftSamples = []
        rightSamples = []
        for sample in irs:
            if count % 2 == 0:
                leftSamples.append(sample)
            else:
                rightSamples.append(sample)
            count += 1

        dest.write('{\n{')
        dest.write(', '.join(leftSamples))
        dest.write('},\n')

        dest.write('{')
        dest.write(', '.join(rightSamples))
        dest.write('}\n},\n')
dest.write('};\n')
src.close()
dest.close()
