-- testing declarations
x: type record
    a: const 5;
    b: type a .. a+3;
    c: var b;
    d: type record
        e: var b;
        f: var b;
        e = c;
        f = e;
    end;
    g: var d;
    h: var d;
end;
y: var x;
z: var x;
