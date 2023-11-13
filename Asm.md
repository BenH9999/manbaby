# Assembler

## Control Flow
- [X] Read in a file.
- [X] Apply the preprocessing.
- [X] Run the main assembler.
- [ ] Save the output to a file.

## Preproccesser

- [X] Remove comments.
- [X] Remove empty lines.
- [X] Map labels to memory locations (The line they are on)
- [X] Replace labels with the corresponding memory location 

## Main Assembler
Note: The machine code should be little endian (e.g. 1000000 would be 1 not 128)
- [X] Extract the 'Operand Line' and instruction name from each line.
### If name != VAR:
   - [X] Convert the instruction name to its 'Function Number'
   - [X] Set bits 0-4 to the 'Operand Line'
   - [X] Set bits 13-15 to the 'Function Number'
### Else:
   - [X] Set the line to the value given by var

## Extensions:

- [ ] More instructions.
- [ ] Immediate addressing.
   - [ ] Define how these addressing modes are encoded.

- [ ] Error detection:
   - [ ] Invalid instructions. 
   - [ ] Syntax errors.
   - [ ] Undeclared variables/labels.

- [ ] Create user documentation to explain how to use the assembler, its features, and the extensions.