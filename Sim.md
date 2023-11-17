# Simulator

- [X] Define data structures to represent the Manchester Baby's components.

- [X] Fetch-Decode-Execute cycle:
   - [X] Fetch the instruction from memory.
   - [X] Decode the instruction to determine the operation and operands.
   - [X] Execute the operation by updating the relevant components.

- [ ] Display interface:
   - [ ] Create a text-based interface to display the state of the components at each step of the execution.
   - [ ] Display the contents of accumulator, memory, and other relevant information.

- [X] Implement operations:
   - [X] JMP
   - [X] JPR
   - [X] LDN
   - [X] STO
   - [X] SUB
   - [X] CMP
   - [X] STP

- [X] Read and parse the machine code input.

## Extensions:
### Simulator Functionality
- [ ] Add options for:
   - [X] Increased Memory.
   - [X] More instructions.
   - [ ] Immediate addressing.
      - [ ] Define how these addressing modes are decoded.
- [ ] Error handling:
   - [ ] Invalid instructions
   - [ ] Out-of-bounds memory access

### Other
- [X] Develop more assembly language programs
- [ ] Create user documentation to explain how to use the simulator, its features, and the extensions.
