-- simple example case from course website
putstring: procedure( ref s:array uint16 of char, f:file )
    -- put the string s to file f
    for i in s.min .. s.max
        putchar( s(i), output )
    end
end
