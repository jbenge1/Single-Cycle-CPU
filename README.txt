The extra instructions I decided to implement were ANDI, ORI, and BNE

I did so by adding a new method that will zero extend the 16 bit immediate field for ANDI and ORI. For bne, i did the same as beq, however to differentiate, i set the bnegate field to 1. (this is broken, and I do not have time to fix it)
