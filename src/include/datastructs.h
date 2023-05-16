//gas base properties definition
struct base_props
{
    float p, t, tc, pc, w;
    base_props()
    {
        p = 0;
        t = 0;
        tc = 0;
        pc = 0;
        w = 0;
    }
};

// contants for Cp calculation


//PR EOS properties definition
struct PR_props
{
    float a, b, aa, bb, k, alpha, ac, c, d, e;
    PR_props()
    {
        a = 0.0;
        b = 0.0;
        aa = 0.0;
        bb = 0.0;
        k = 0.0;
        alpha = 0.0;
        ac = 0.0;
        c = 0;
        d = 0;
        e = 0;
    }
};

struct CP_Const
{
    float A, B, C, D;
};