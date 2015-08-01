-- strip non ASCII content and all control characters from the input,
-- except newline characters, while copying from input to output

ch: var char
while ~eof( input )
    ch = getchar[ input ]
    if ch < " " -- control characters
        if ch = LF
            putchar( ch, output )
        end
    else        -- not a control character
        if ch < DEL -- printable
            putchar( ch, output )
        end
    end -- if
end -- while
