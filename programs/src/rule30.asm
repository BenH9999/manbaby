board:  VAR 32768
		LDP #0
		DIS #1
		WAT #1000
		LDP board
		STO backup
		LDP #0
		STO board
		LDP #32
		STO i
loop:   LDP i
		SUB #1
		STO less_i
		LDP backup
		BSR less_i
		AND #7
		GET #rule
		BSL i
		LOR board
		STO board
		LDP less_i
        STO i
        LDP i
        CMP 
        JMP #9
        JMP #0
backup: VAR 0
i:      VAR 0
less_i: VAR 0
rule:   VAR 0
        VAR 1
        VAR 1
        VAR 1
        VAR 1
        VAR 0
        VAR 0
        VAR 0