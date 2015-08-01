-- testing more declarations (harder)
global: type record
    up: @void;

    x: type record
        up: var @global;
        a: const 5;
        b: type a .. a+3;
        c: var b;
        d: type record
            up: var @x;
            e: var b;
            f: var b;
        end;
        g: var d;
        h: var d;
    
        initd: procedure( self: @d, up: @x )
            self@.up = up;
            self@.e = up@.c;
            self@.f = self@.e;
        end;
    end;

    initx: procedure( self: @x, up: @global )
        self@.up = up;
        x.initd( self@.g, self );
        x.initd( self@.h, self );
    end;

    y: var x;
    z: var x;
end;

main: procedure( self: @global, up: @void )
    self@.up = up;

