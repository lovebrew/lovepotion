-- make sure love exists
local love = require("love")

function love.nogame()
    local resources = {}

    local function decodeBuffer(buffer)
        return love.data.decode("data", "base64", buffer)
    end

    local console = love._console_name
    local is3DS   = (console == "3DS")

    if console == "3DS" then
        resources.ctr = {}

        resources.ctr.bottleFill = decodeBuffer("\
        AQAsAABsAJQAAAAABGADsAERAAACQgAQKaAAOczMzDOvGK0gB1ggC4BgDwAzwxcgK11AYAN3YCsA\
        MAMHc/9WoM97NgCgr/DHcONw33UBoDsCsP97E2FfAXU/cYvwD4AG4AMC8L+3EA2w/zMEJC8D8v9V\
        AeNXBvj/NaNtFgAljzXbdgGkvzXrNwAmD7YANfdYCCX/AHqHOGbzNut4u+U/GQkgvxBeMJ83l1Ou\
        nzo3VRouu3oB7n8aAG7fVW8Hqz3rVG8HegJtS3kHLYMQWrB/WlKvn1MAL4/371OvzzT1BGd7AbgL\
        Bv//eONVAGkLdaFfajUHIK8QTP//NW+3VQAvnxutL9d1b9d7A283P/82oAeuADefdm/vVhBi76sQ\
        AHc7eOt13QAvg/jjNqiLAz//+UN26PPvAPAfEFM//39/G6/Tf9/3D3crWhov/3oDb//3cxoBp1t6\
        1QDgj/BPeW//GS//WRBs7/9XVK//eWfzU+/jAH9/f9+ruQs1L/8bA+93VRBTr8cQAjZHa1Jv/zgT\
        GK//eKAX9+92UKhDAX//MFt3AC//A/f7TrWgzxcBKKsHf/9WAG//LeAH1RA7f/82CzgA7/92AWUT\
        Vy//dTcBpfMAfv9281kv/zlvJ294ZrsAfvMZEGbuvxAS93t/Xzo7f2wAL/9wFwBwHwL//wDwXwTw\
        fxAwP//6/18Av38C/vsG8H8QjzA/bBDub/8X1Qcvf3//Fi/7dgAv/01v898AMBsQPn//beWfAT//\
        AzCrEIF//z6zqz9bd+ofTy9vbgAvXwH//1ZQL7cYL7t4YAcwZ2/bEDWvHw+0Z3dl6z/HTgAvj//f\
        ei5vszWvAH//AvXHT2ZjeNcBJhMHf/8YAGcLcKEnNvcQcT//VVIvfzOvXxpvh1Mvl1V5AK9/US+3\
        ce+fGWAzbXIQRm9fEAU1c3inE3+Hca93tvb3UgBnPwC//zIAb98C91N59WejN+s33wDwXzNoE3oo\
        I9p4FxBef/83AOe/AHcfdS9fVa1vXzRvZ3oCbx8//xtvp1Z2L7d7759WEFyvdwx213TbCaezf/9V\
        AGkLONcaaOs497cD//+AACjvcBc2AGlTuSMA//+1eVtXBCC7EEA//xwvU3wvV31Yrt8wC3AXAHZ/\
        AD//WQAvn1cdL7N9r295EGovHxAL96956+o597oHAD/feK/fWQRpu3zWAaoTBv//HWsjeQBrCztD\
        WvZrSzr38DcQKr//PARkfz9/XL4AL189AC+DP/80X39X/5979RCHL18QBbl/Ou9/d1sAavt+6z+t\
        AL//Hm/ffACv3wL7Xzxr690AMGP7zz1sczvrECN//31jV6oAPyddACQzgADkPz5vtz67AGRfXm/X\
        AHBTEKbwEz3r+//jb38Hq7cA//8fAOyLAv//vMsAP//3cBcxM3CLEBT//34BYpMDP38AsvPdAH//\
        c4sfAa//B7RzEKfwhz+gB/v//wDwHwL//3/XAP//XhAVb/sB8t9Wfm//XQBv/x9vez/nfdoBb/8B\
        82Meo7dwj1wAYF8er6NrPBCk7/9+AG//PCt+9wPwb+s8h7wTfAtcAG+7PhAYb1cQBXKTbVtv/zQz\
        WgAv//QzWqAvalkDL/8C9HtZoL88r/98rQBgvxwv/1ulCwl//zsv/1t7EH/v/1sAb/85+1hvBzoH\
        tToXGwBu/wB/t30v/x1v/1d9oA8d70ceAu8/8E8QRjALtrYbWm//NjsaASXbf/9a1gBv//BHWaBf\
        GQcl+wF//1ndBG//95M4p7sBsL836zev/3VXAGBfAnD/Bn//Fi//QKi7blbhNwF//xVgR3//MFNU\
        qxAtr/90ACP/FQlv/3atuzULVjYAZP935N8YBOX/APcTeqpv/3kv/zlujzruh3jt5d8BMLMD9/97\
        L/9wlzsQRy6PrxA29X8TAaqDQOq/8A8I+kvwv35Rb/8Buz+wL/APCPtLP/9x2AKhvwrx/zMQjyzv\
        HM6wAAAAAA==")

        resources.ctr.bottleLine = decodeBuffer("\
        AQAsAABsAJQAAAAABGADsAERAAACQQAQHaAAPP///xAgA02BIAfd/1AABnL/ICCLVV6gfwwgm2pg\
        jzIgp1LPIKv9IK/u/wDQAIS6INPVoCMwMwFwAvcBoP+bqiE70iE/CyFDMyFH+6oBYT9gIXOHoMOq\
        IYPJWv8CEADBYOtxDk0hyxmtASDr7CHzwKHnMMd5AGH3VQKiDxsiKy4iL+YA4VOqdT9AIltVok9f\
        ImtkVf8DEABaIrM/oqclIsNqDqK3sh7moivfIuvOqwGi33wjG4CjD4BgDwDyTu3wLwHwP3LewGGn\
        sA+5ASOfq3MeoSPboKMroGATAXbfrTD7f+D/fAGg/zHrYKRPVVEka0ABo4u5ACD/wAGg/3/OYfPy\
        CwF33zLrMvPzC/R+df3iZwD4PzPrqKTDhWEn1wF0pjLrJaWnP2MTADL/ADUqrTPrm6XnwGQTAXj3\
        M2Tzb/9k83kv0uBXAPlHNes189oAcr8A9bYZZPO198EAJfPV9eUntesAtL8GegcyZ4P9ZvPVeo83\
        CwwAZvvuAaJ/gWfzav9n87gLABBg4AAkAC8fA9VrlzyX+i9PQi9T8C9XVeMCLdPGb38Hb4dXL59V\
        02+X+q+fty+3KC+7VfIBLtvpai9xL+sBb+Ne2W/rfG//P/MwGwC6P8uqb5/WL6dBL6sRb6sC1e8z\
        MPucL8sKb8tObyOqEAT4Wxcv/0YBrt8nL/cE2hAvqfM1rzMDLv81606v15urL+ftb+cKA69fRKa7\
        A7+zu39iXmbz/3L280FnEz//VREAL5/waBP/6BPGAG/LrwL/gyVn8wFn8wAwv7E3eAtaeeB/JABn\
        szjrA2jrWvUEYX+487kLADift2mT8mkTqhBKP/8bb1dSBG4niW+3raoAbofNb9e9byf+L+fEtQBv\
        v/Hvj7/m+C//9S//Wh8v/7Wmu78faG/jIuoAbxMAf3s//p0Ab+92b/+LtS//DO/PEAM/dwgBr1dN\
        L4dVkq+HtC+XtG+XHgkvb1WSYk9ZLA+KLBNYLBdWPywbwCwf7wEsHzp/E9YAKs87V+4sYxxgT3//\
        Yaosd3wIKbffLQ/drQ/Fq2EDmC0nqK0nwGEb///d8BdwJ9ctZzBbsGfTCCq3VcEuD9ngp+RiB76u\
        J1qmLjeU7jc7s/1tSCCqLlc3rUd6Lmddrmc7qy53FQert0ovB2ZtQzDnVYQvF6EvG4UvH1EBLx9V\
        vmyTF29Ljy9XQQmsl1UFEAPtQx9oEwYv/7IAb9/1APgTAv9jAbgT/1/+aRP/6RN++ABpEwL56wG4\
        6wA5ogG56xBFf/+drW/fXABv3xEHrqt/9vyv/1bOr//nb/9r4CN/tw26b/NjL/8AP4M9Qz1v5Oyt\
        r6M9b7cDaj8/wxSv72/S7U9/2yFsU3AL978wY1Y+bY/2rZfLAGdfB3//besvcwA6iz5aqa5fGW+b\
        PJtdTQGvpwIQRaf/CX//BHYTDWgLV2Pv/z1oC+9oCwC3fzcLVW1vxxlm6+TnC/9oW2/p5+sBcArL\
        6Ov/ggA473kTtzkvb2+3Arnr/+jzOQs5E2oUb9c46yFgAwAQWCAAEKov/6ivy/1v624v9/K1AG9f\
        LQevdwD3981vewkvn1WAr4f3b5OfL7cVb59V4a+/Ty/PAgBoU6ivz67/7zMv8Mxv4zBzAXC/fdUA\
        aQM//yuv+8Ip8/4p921EaTMA//8baiM5Y2SqL9U5nwQxf54qkwdpYx4AKdO7OesfoAcQNb//dPMb\
        ZQs+d6q/S09mC59mC/9nG/zVAK6bNOuaZOvnAC7DH+TztgF+5vZmEzXrwm8fNfMrqu/z/AGvN/en\
        E4BnE/+3Z/PyZ/M4CxBuwwE36zbr6jbzATbrEHkwHz8v9/Mv+y6/L//tAC/fcBcAcB8C/1MA8F/w\
        f9UA/1//n+wv9yBvtCoDL3++ADsaKuAHALAfEDL//wD/X39/KK8Ab8f1L/dCpdP+67XrADAf/QG0\
        6wA//gDwXwDwfxCD8A8A/98rb0NfFy//4wQv/3BXAHBf8B8G/3+vAL//RgBv/0qgBxAy//8Ff391\
        k78Af/9CASTrAD//tesAMF8A8B8Qg///6gH/fwR//z879W//Bm+nW68v+7pv8/gAb/sBOnMDv38A\
        v9+qADsT4S//Gy//hy//AeoQMy//AbTrBD9/SgDv/xtk8wHfZPMAf//1ZfMANV+187YLEIJ//1U/\
        L+MML+fLL+sVL+9V2C/zjy/3/gAvU/MAr2dV9+//Oy+7gi+//S/DVvJvpw8vz8oAoF8AP8aUqi//\
        2i//Gm/r/G/znbov/1Jvs3oHP//kb/9vqi//IO/Txi//EC//Q7sEb2sFEDcqh7TztQv/AC/PNQtV\
        xm/nQwAvs9hmE/5mE+u16wA2EwL1E5Rv2xpl8/aC/T/rtfO2CwBwf/brP//PZvOvA////Wbz/2bz\
        twt2n7dWdcpnE3FfEGg//0IvHxYvI1XW7t9ZLzeabyf5ASh/VQMAbnNwL38uL4Pubq9V4i+PtABv\
        Z/sAr3/vL8dV9ACvn/kAb7uQL//VYONW3G9nTwBvHwQA7zc/5vatb+uDL/88b59w+8Qv/1YNL/8s\
        EElo6xpmEz/XUOoCZhMFvtN/X/yvaPTm6/+rZ8/VACdjQm+z2mfz//9//mfzd0+4C3/rtvO3CwA4\
        H6o3C8Rv9ywAJ/PiaBP/vugT7wBoEwd//wC46z//OKsx1RBJaAMBf/8YBG3TrS9/Zm77qj//Ki+P\
        By+TxS+X8dXvJz//Cm+njC+3zG+XVP5vn0gvy/cvz+D/1QGQADB7qi//CG/j32hfWkwv/x7v8wCw\
        Vs4AL9Pyty/7NC//NxcBL/8wsz9jVRECbx8BCa9XJqpPiCpnVeEBKkcoKo9kqduOKp9qrwgo/wB/\
        /8ArT8Cqm72qK1+bAam/YqoP/KuPvqorp0cJqU8DECAp+yIBb0cHq27H7GbPhAmk/7Hk/zUPVrwB\
        rr+fL+B6r9s08wy1b/T1AG7/AbcTZm/bCGfza/8AKF+3899v30zljwD/h7c2s0uvrwB/LwRoEz/X\
        ATgTev/o83kLMi8AOJ/3aRP/6mkTAvnrALjr/2jr8mjrcK4AKYOtakP7afMAMN+58yq1anPFahMQ\
        Pr//TgDvfz0AL59VDi+z1y+3mG8jhm8rKvj/AhAAmm//TS+vErUvs+Jvs7+iom+v+wEvf6o/Y9Yv\
        +xMv/0Cmv4Wqb0sNL/8DAC9/tC//569tJwjvn1Nvrz9jADlXAD9zWiBmi/JnTz2vrmdf/dcAKB//\
        t48v/01hU7BnMSdd027L+W7T8E8B8YagoEuqDPAG86/3kS//KQkg9+TqL/86M3qT8m/7BS//RLUB\
        o3PAad/6or8vVyxsD1bwr2NfL28CACrPM+dp16ALAP//Am+3amBPAHn/AnLGWvFrF/9hJ3//HwBq\
        v5WqYNfoLTsmEAZp/ylla9jVAu5/clsTYquNLw/D7YN1NWv/dkcAPZ8LL0cebmdqgG4XfyLaDGUb\
        7C//c6sJJff7aBcUb+vgboNxt60BeHZgL//NASijMpNRqE/qAPjWf9c+hxkAbn/cr0Qxt2/z5WAL\
        BDd63gGpiz/ruPNftG/wIGkTAD/fuRP/tjjfdYOmK7nrAD/iHW+/sABvi6s450xv964AL98TafM/\
        93VA6gt5X7Z3gWqnDWoT6wC66z//OusOauucBCYv+ttfTWuT4msTADtWuxMDe+sQI3//bg8vUzaj\
        kG8fP/918732ZrsA/f8A//90I7dlGz5TytoAb38Avh/pAG+zf8t/ACeL1Ksv/wwv/z9vuzUAJWMB\
        9P9VnGx7GQAvn3ls+xAEq6tWe2/zcwBqHy+v+35DULov+9IBr3v9ezx/jAMsfwa9L/8Vrbd+JwG2\
        f//+/GdTakdvMzczG25/De3zpdov/z1rAg5tSzk7h2+bA7eny4QAZ58y35QAKXcGfocBcrdqmgDk\
        S7/HZG/UkABv3Jitb/RwL/wHZSt/t/pv/10zL/93AS+ff989t2FlB2sYaTsQAn/bHG+XMABvn3Xz\
        VSIvx9tvb/QArb//bcPtAL0/AD/XATtrgGxzP/8+r/u1NdN+AaI/Ab/m2G4nJ2//Vcsv/88v/xov\
        /yPuA+owIwD7/zA7Vi//Ci//Jq0KK6cHrwdnZts/qxgJLHO1f/+fAG/vOHtCr+fPb/+qN3ddAa+/\
        yAAgX+wDbk8Lq6+/FQIn/+Zv894AL+8+91+OL++NYp8BPv8C+hMFvqcAeuu2f5IMavM/9z8AJNc7\
        6w//a+u2x7/fu/O8C7gTf8d3G3UZaxP/3zsLeW/3EABr719MbGPKAGvrvBMI//+86xAVP/9WJ29P\
        gQAvnyNn+wD/V3iqaQ8YAC+TpW0rS6/7YrVvz5YALic//8Zv794Ab+O2eXu7paN/R/9lvz//YrUA\
        ZE8AEJ/gAAG7E3hv2xirh9e/ljvrom/zSwAvrwM/f7vz2rwLAD/fiqwLfMcnbOuBvQAvY//s870L\
        Az///UZmrQvfADAffQviEBOstz9zcAcAM58AsB++P/9GoAcAMz8AsB9/1z//vq0AZB+Sb/9ar/+w\
        q2pv/1WRr98Br//Kb//7L/9fBG/vExCgL/cDP4u8CwA/3wDwH7sCv1MB7At//7vzxqwLADzO/70L\
        AD+f8B8D//+9C///Pae9C6oQE3//GK+3/W//wQBjv22qb3sQL//0AC+vY2/jw9VvawAzZjhv57UA\
        b4OMb/dqFwEvo3PG8wBjz39j0wi1L//SACPzAX//BARjb1JlI3XiJTexh7QfuqSrBWVTujTfAxAT\
        pAMQcj//f1dja+vDqgDr6/8Ba39Oawvib78FtWrrkesLAPAK8wBv339sC3oIb/+88/uDP+9tbOsQ\
        7QQvcwG86xAYe+str8d/h1sv/1v8AG/fgAeutwA0lnuvb3QbdoekJ7Tmf4N/AG+PP7+hta+/Cm/v\
        NSsib/uGr/erdItvL/j7AG/fSa/3ADVWVeqlh+xlkyAlq8TlB1r/ByRzWWXbNjqqpg8a12Zb9lLu\
        JnNxAOY/v/9xb17bJqtcpm82UxBt//938wqqaAvbBG1zA26vf6BvIrVvH6EArv848wIvU1YvV1qm\
        b0txaPMAfvbqAa8P/rqqE8RqE7rrAX9a/W8zh72vZ/xr87wLATvrf3/7avP6uwv66xA4ea+/9zTv\
        9m/jKdoJLmN/X7ymH/afVq+X3Kpvqwcvt2QAJpfYL89otQAmr90AZsMC/4MGL99gr9dr0GbnP/89\
        AS/jC2gTNxNWjKgPKCgrAQAndzbnxdoBoL820x+gO3//xiiLfquoC6Qom/QAYX8PqKcxc2q5qLcA\
        9/76aG8WKPtatah7pWg7AX//Myk7BAQpL6s5Du2pE7Ipo3kBqNMx420PACFfMqezASkbB3//iSqb\
        V2Oqj0IqqyUBoL8zjwB5X7q6P9pqhwfw/wB//7srs6O/qzOgYAMBdD/wLwHwPwb///DvqjELuwFl\
        Pwdsc9qsc+7bCGL/MutCrY8zC4kBo78z61bcZAs9bI//BKP/NQsz7QGuPzXrNfMbbOdwn3lk8+1+\
        FrULA7AK6mXzdhNaZhP1f/826zbzdxOMbifFAOFLamhn8wIwtt0Artf0ZxO53Whnf/8P77M+0zfz\
        QW/b9TgLOBMB+P8APyu8b6vcaOu/OPNW77M56z7jeeu6CxBucAeqCbgDLi/Xbq/Xpy/n3q0Br+f+\
        b7sTa79/ymwv2GqYCS5ff2LAL3/hr2/8rQBvHwUvpx2vpwD/ojkv11ZRr9diL+d6B693AP1if78v\
        f5SvZ39/AXD/8C/wPwjw/7rw75RhEwFx/zHrYq+nUe9iEwgx/78z4WMTMusy8wD9P22YY/N9O0Jk\
        Ewj9C+Ivx3ynr8c1CzUTDDXrHVAwAAAA")

        resources.ctr.cloudCircle = decodeBuffer("\
        AQAkAABCAEIAAAAABBAC8AERAAABQgAQDaAAAv///zHvNK0gB3wgC/tgDwUyAwUge1VSYH8PIIey\
        IIv4oH9VBCCbaiCfDCCjdSCnJOn/UADQ/wDQAOT/1QLQAACy/wchL1MhM5egg2rRYTfwPywhW02h\
        T2TrYMcI8AJz/30iG32iDzGDfmQBoT8w+zEDsTs0W/G+++1hQwfw/wB1B5diA3KGD2NDd/hiwwF1\
        PzL7BQAjbwMyIrMDbnZjQzM+6gMg/wV2HzP7BNYAYR+0AzRkw/tkwwF2vwPtB6DrEIVwhwB583xq\
        RwD+/x/vH1YgLz8lL0PiYAM+ohGtL1O3L1fWAm6HMD/UYEOuEDowArdlA/9lA/U79Rbi/WVDcAsG\
        9PYBf/81+wQ//9WhP1W4pq8DAGE/fGbT+2bDqhB3P/8Eb2drL2/Q7wcMqi+D6S+HdgGpfwVvt1Oq\
        AG+/si/XEC/b+S/f5F3/EFQQANABplO2+wA//+pnA37/ZwP//wM2vrcD9zv3drLaZ6s3Ow8AZ38Q\
        br//Vm+/l6sAL58HL+fSb9/7A293P/9XLW/vTQBgH2RpYzk/EFkwAv/3Q//jA/dDAvd/AT//N/sE\
        P/+3++oAP/+3+xBkf/98b+d8AG+fduqvhwQ4f3f7LQBv/wiop/vr6UMQYjAGt0N9b9d9AG/ft/v6\
        BD//t/sAP/+3+xBlMAeYb/9V1a//uBcQL//5b7eyA2//rfh3U2/3BQEoc3//5gBv/1V2b/8ML//q\
        EFVv/5dv/39UAC//Nzs3Qzc7tr8DP/83A9X3Oz//dy//6y//DRBl70uqDXdra2/HBK//0ABv3zSr\
        L//7b/d9BGiPAwGo5wb//1a4b/8RL//VAC//OiMgqqm34m//JS//4hBCb//ir2U7JQDv/9AAL/82\
        uzbDdrvvtvsCP1t2A9ZmA/a7tfsQhXAHrQG5c9avv7gAb7+503xqQ7Y/1zQHKdsBf//QBG//Ozdq\
        q6s76m//DC//dgErU3//VeUAb/8GL/9Tq5uzL/9V+Wu7EAmv/1Uv/5iv/23SbIsQAn//+2FDCX//\
        mGID+jK7MsMyu/O/cwPqYzsM9WL7MwMC8/9/D/tu+zSgf3fWbsM+v/6vAwDvFzP7An87qhC0cDMI\
        AayPLS//Ta1fZK4v/3UJrM99bc9+HzC7ZPgBrY8w+zEDAjE7F6cwAAAA")

        resources.ctr.heart = decodeBuffer("\
        AQAbAAA5ADIAAAAABJAD4AARAEAAQQAOIABT////ZyADavsgBwVw/xcgc06gb3yqIIOPYH8DII8q\
        IJPAqSCXUiCbviCf/v9QACbw/wDQAPz/AtAAAXH/kKohM32hL1khQyEBoT8BrQBha/8C4L/RYJdx\
        Jhth01r1IeOVA6D/BPMnGQBii+mqIqtlogMGYrOuIsMZ1QHiv3A7NiL/cCMD9AWi/1UVI3NpY28J\
        I3+8I4NV96N/DCOTeyOXHyObVZai980A4v/yBOL/MSQrVlCjg2AkO0wBoz8z1wOrY8vupG+iJIM4\
        AvL8AyQTqgb1/6AlixJk69WljyTVZZv0/t4luxplG8IBpb9qBAej6wr3cwhnN3tnP1WtJ08tJ1Px\
        Z1O2stBmx2v+AWZ/NyfzJ591ZwO4n1IcJ7fUZxf9/xAUkADWrgGpU0AqS5ypoxAUspcFdPL+rQGs\
        U3ltQ/JpuwA735psy/U1170PAzv/9lt1bdPIAGg/qjY3EG1fRw9oE09vB27WACenP/9+byd6BCqv\
        P/9fvm+HNK73fv+4b33XPxqsVf8QSJAAa2/PfgBt834A4B+tAv3aZ25vQwAuc3f/xA9n+6r//1Vv\
        qwQv4+IAaR9rrW//BAGqS9ABb/8B+Y88L/9d+mqDhwAqw3Cnepe/b/9bGS//3BBDaEN/f/evf7/f\
        rT//9i//IgAvbwC/z98AL+eqNetKL/+Mb+eQb+8P1RAAZ88GOOMuCalb6W//NC//VegAaJ8zqivn\
        b/8yL/9q5gMv/wA6gyABqpvRb/8Tqi//tQBo/waq64gv//mqavtUCa//6RAbr//aL//Y1mcPAb//\
        +W7XsS//M8cGrW//1y//yS//NDch7tdqEwDu5z//uS//zi//FfVt978XADezECuwDyMv/7yoJ657\
        y3Qv//UAa1O53wD//8KrbB8FL/9qr//sAKj/ArvHWhwv/6IBrAsG///4b/88rS//wa///ap/tZNS\
        L/9rzKAXAX//UC//qGBXMGdWSC//AgTv/8hv/z37y6ov/zgv/4Qv/wgAIj/8qmCf7y//ei//I25H\
        QaoA7dcKAu337i//Vy//JsAQLObnEOBwAAAA")

        resources.ctr.noGame = decodeBuffer("\
        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")
    else
        resources.hac = {}

        resources.hac.bottleFill = decodeBuffer("\
        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\
        R3Qc72F1aoRdSCWnrEXf7gp01UpPy4xXvU3KbAk/IwQ5Q2IqC2tKA1Iiam+NMaH/5BpjUrFLvtI7\
        wWi+UkjraggkgoNhm0wN+IBGLS0aSZwaNBrZZ0KYfxf0ANGcRXKH/wE26paEeAWBfgAAAYVpQ0NQ\
        SUNDIHByb2ZpbGUAAHicfZE9SMNAGIbfppaKVBTaQcQhQ3WyUFTEUapYBAulrdCqg8mlf9DEkKS4\
        OAquBQd/FqsOLs66OrgKguAPiJOjk6KLlPhdUmgR4x3HPbz3vS933wFCs8ZUsycOqJplZJIJMV9Y\
        EYOvCGCQZhhxiZl6KruQg+f4uoeP73cxnuVd9+foV4omA3wi8SzTDYt4nXh609I57xNHWEVSiM+J\
        xw26IPEj12WX3ziXHRZ4ZsTIZeaII8RiuYvlLmYVQyWeIo4qqkb5Qt5lhfMWZ7VWZ+178heGitpy\
        luu0RpDEIlJIQ4SMOqqowUKMdo0UExk6T3j4hx1/mlwyuapg5JjHBlRIjh/8D3731ixNTrhJoQQQ\
        eLHtj1EguAu0Grb9fWzbrRPA/wxcaR3/RhOY+SS90dGiR8DANnBx3dHkPeByBxh60iVDciQ/LaFU\
        At7P6JsKQPgW6Ft1+9Y+x+kDkKNeLd0AB4fAWJmy1zze3dvdt39r2v37AY3mcrLb10WeAAATM2lU\
        WHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBD\
        ZWhpSHpyZVN6TlRjemtjOWQiPz4KPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIg\
        eDp4bXB0az0iWE1QIENvcmUgNC40LjAtRXhpdjIiPgogPHJkZjpSREYgeG1sbnM6cmRmPSJodHRw\
        Oi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICA8cmRmOkRlc2NyaXB0\
        aW9uIHJkZjphYm91dD0iIgogICAgeG1sbnM6aXB0Y0V4dD0iaHR0cDovL2lwdGMub3JnL3N0ZC9J\
        cHRjNHhtcEV4dC8yMDA4LTAyLTI5LyIKICAgIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUu\
        Y29tL3hhcC8xLjAvbW0vIgogICAgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFw\
        LzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIKICAgIHhtbG5zOnBsdXM9Imh0dHA6Ly9ucy51c2Vw\
        bHVzLm9yZy9sZGYveG1wLzEuMC8iCiAgICB4bWxuczpHSU1QPSJodHRwOi8vd3d3LmdpbXAub3Jn\
        L3htcC8iCiAgICB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iCiAg\
        ICB4bWxuczpleGlmPSJodHRwOi8vbnMuYWRvYmUuY29tL2V4aWYvMS4wLyIKICAgIHhtbG5zOnBo\
        b3Rvc2hvcD0iaHR0cDovL25zLmFkb2JlLmNvbS9waG90b3Nob3AvMS4wLyIKICAgIHhtbG5zOnRp\
        ZmY9Imh0dHA6Ly9ucy5hZG9iZS5jb20vdGlmZi8xLjAvIgogICAgeG1sbnM6eG1wPSJodHRwOi8v\
        bnMuYWRvYmUuY29tL3hhcC8xLjAvIgogICB4bXBNTTpEb2N1bWVudElEPSJhZG9iZTpkb2NpZDpw\
        aG90b3Nob3A6M2Q3Zjk3OWEtZWQxOC00MjQ4LWFiN2UtOTJmZjg0ZTliYzlmIgogICB4bXBNTTpJ\
        bnN0YW5jZUlEPSJ4bXAuaWlkOjI4NzQ1OGYwLTEzM2UtNGVlOC1hZWE2LTljODk4ZDdkNzhhYiIK\
        ICAgeG1wTU06T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlkOjQ2ZTgxM2E1LWVmMzQtNmE0MS04\
        MTU4LTUzN2VmNjc3NWE5OCIKICAgR0lNUDpBUEk9IjIuMCIKICAgR0lNUDpQbGF0Zm9ybT0iV2lu\
        ZG93cyIKICAgR0lNUDpUaW1lU3RhbXA9IjE1ODk2NDY0MTQ4NjU5NjEiCiAgIEdJTVA6VmVyc2lv\
        bj0iMi4xMC4xMiIKICAgZGM6Rm9ybWF0PSJpbWFnZS9wbmciCiAgIGV4aWY6Q29sb3JTcGFjZT0i\
        NjU1MzUiCiAgIGV4aWY6UGl4ZWxYRGltZW5zaW9uPSIyMTUiCiAgIGV4aWY6UGl4ZWxZRGltZW5z\
        aW9uPSIyOTYiCiAgIHBob3Rvc2hvcDpDb2xvck1vZGU9IjMiCiAgIHRpZmY6T3JpZW50YXRpb249\
        IjEiCiAgIHRpZmY6UmVzb2x1dGlvblVuaXQ9IjIiCiAgIHRpZmY6WFJlc29sdXRpb249IjcyMDAw\
        MC8xMDAwMCIKICAgdGlmZjpZUmVzb2x1dGlvbj0iNzIwMDAwLzEwMDAwIgogICB4bXA6Q3JlYXRl\
        RGF0ZT0iMjAyMC0wNS0xNFQxMToyMDoyNSswMTowMCIKICAgeG1wOkNyZWF0b3JUb29sPSJHSU1Q\
        IDIuMTAiCiAgIHhtcDpNZXRhZGF0YURhdGU9IjIwMjAtMDUtMTRUMTE6MjM6MTQrMDE6MDAiCiAg\
        IHhtcDpNb2RpZnlEYXRlPSIyMDIwLTA1LTE0VDExOjIzOjE0KzAxOjAwIj4KICAgPGlwdGNFeHQ6\
        TG9jYXRpb25DcmVhdGVkPgogICAgPHJkZjpCYWcvPgogICA8L2lwdGNFeHQ6TG9jYXRpb25DcmVh\
        dGVkPgogICA8aXB0Y0V4dDpMb2NhdGlvblNob3duPgogICAgPHJkZjpCYWcvPgogICA8L2lwdGNF\
        eHQ6TG9jYXRpb25TaG93bj4KICAgPGlwdGNFeHQ6QXJ0d29ya09yT2JqZWN0PgogICAgPHJkZjpC\
        YWcvPgogICA8L2lwdGNFeHQ6QXJ0d29ya09yT2JqZWN0PgogICA8aXB0Y0V4dDpSZWdpc3RyeUlk\
        PgogICAgPHJkZjpCYWcvPgogICA8L2lwdGNFeHQ6UmVnaXN0cnlJZD4KICAgPHhtcE1NOkhpc3Rv\
        cnk+CiAgICA8cmRmOlNlcT4KICAgICA8cmRmOmxpCiAgICAgIHN0RXZ0OmFjdGlvbj0iY3JlYXRl\
        ZCIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo0NmU4MTNhNS1lZjM0LTZhNDEtODE1\
        OC01MzdlZjY3NzVhOTgiCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rvc2hv\
        cCAyMS4wIChXaW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMC0wNS0xNFQxMToyMDoyNSsw\
        MTowMCIvPgogICAgIDxyZGY6bGkKICAgICAgc3RFdnQ6YWN0aW9uPSJzYXZlZCIKICAgICAgc3RF\
        dnQ6Y2hhbmdlZD0iLyIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo0MzcxZmNhYi00\
        MzEzLTM3NDUtOTI1Mi0yZWZkYzlhODA0ZDYiCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFk\
        b2JlIFBob3Rvc2hvcCAyMS4wIChXaW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMC0wNS0x\
        NFQxMToyMzoxNCswMTowMCIvPgogICAgIDxyZGY6bGkKICAgICAgc3RFdnQ6YWN0aW9uPSJzYXZl\
        ZCIKICAgICAgc3RFdnQ6Y2hhbmdlZD0iLyIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlp\
        ZDo4M2NlZmFhMC0wNGI2LTRjYjItOWIxNy02YWIxMmY2MmVjYzMiCiAgICAgIHN0RXZ0OnNvZnR3\
        YXJlQWdlbnQ9IkdpbXAgMi4xMCAoV2luZG93cykiCiAgICAgIHN0RXZ0OndoZW49IjIwMjAtMDUt\
        MTZUMTI6MjY6NTQiLz4KICAgIDwvcmRmOlNlcT4KICAgPC94bXBNTTpIaXN0b3J5PgogICA8cGx1\
        czpJbWFnZVN1cHBsaWVyPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6SW1hZ2VTdXBwbGllcj4K\
        ICAgPHBsdXM6SW1hZ2VDcmVhdG9yPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6SW1hZ2VDcmVh\
        dG9yPgogICA8cGx1czpDb3B5cmlnaHRPd25lcj4KICAgIDxyZGY6U2VxLz4KICAgPC9wbHVzOkNv\
        cHlyaWdodE93bmVyPgogICA8cGx1czpMaWNlbnNvcj4KICAgIDxyZGY6U2VxLz4KICAgPC9wbHVz\
        OkxpY2Vuc29yPgogIDwvcmRmOkRlc2NyaXB0aW9uPgogPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4K\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgIAo8P3hw\
        YWNrZXQgZW5kPSJ3Ij8+sWppPgAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAALEwAACxMBAJqc\
        GAAAAAd0SU1FB+QFEBAaNrpJ5+kAAAT3SURBVHja7d1Bkts2EIZRqMs34gV1QZ4J3nk3KtEjiOju\
        9/apjEl+/MFJJXnMOQfweeESgLhAXIC4QFwgLmCMMcYfl+DzzvN8Zvy5j+N4unuf8/DPuXqEI0Bx\
        iUh04hISghOXmMQmLkEJTVyCQmg94xKVyMQlKAqHlj4uUYlsVyEs3GPLJSoLZrmEhfueLC5hCUxc\
        ILA8cVktsj0L4WKCYyGkeuGGi4jALBc4FlotPCOWC/rFZbUQF3gRiwvax+VIiLjAC1lc0D4uR0LE\
        BV7M4gJxgbh8b+FoaLlAXCAuQFwgLr/MQFyAuEBcY/ifUyMuQFwgLhCX7y52t9MzZLmgS1zWiyrP\
        juVCWJ3isl44FgoMz0rOY6HAyPyMhIuHsJrGJTCyPhPhYiKs5nEJjGzPwGPOme4C+7eWRSUukdH4\
        xBIuOu6x5bJgohKXyHAiKRaXwEQlLpHR4Ps53CzcK8tlxUQlLoHhVNEoLpEJS1wCE5W4RIawxCUy\
        Ud3Af/0JYYkLxAWIC8QF4gLE9Y/fbCEuEBcgLhAXiAsQF4gLxAWIC8QF4gLEBeICcQHiAnGBuABx\
        gbhAXIC4QFwgLkBcIC4QFyAuEBeICxAXiAvEBYgLxAXiAsQF4gJxAeICcYG4SjrP8+kqIC68sMTl\
        IUBc4MUlLjcfcUH7F1i46bjW4gJxeZPimjeMS1iuvbhAXN6cuAet4xIW4sKLTlxuJu5J67iEhbiw\
        XuJy83CPWsclLMSF9Sr6Igw3C/dMXMJCXODFWCwuq+UeistNwbEQvChbx2W1BCYuN4FG99axEMRl\
        tayXuFx03OuscQkLcUHjF2q4yLj34hIWlgu8YBPFZbXI9iyEi4nAHAvBN5fVwrOxeVzCIvMzEi4a\
        +OaCVC/icLEQWJO4hIVjIXgx54nLalHpGQoXBYE5FoJvLquFZ2qDuIRF1WcrhAW+uSDVCzw6/qER\
        WNm4hEWHZ86xEKrEZbXosl4hLASWPC5h4VgI1itPXFaLjoGFsMCxEFK9+CPzDw87P6MhLHAshFRD\
        EJl+WMgUWAgLHAsh1XrFzj8cZA4shAWOhZBqvWKnHwYqBRbCAsdCSLVeceffHCoHZrlgt2Oh1cJ6\
        WS7Is1xWC+u1IC5hgWMh3Lpe4oIdlsuREOv1fgOWC+5eLqsF11qwXHDnclktWHgsBK4NjrjgruVy\
        JIT/a8NygW8uKBSXIyFYLtjuu0tcYLlAXMCruPwyAywXiAsq+umUJy6wXCAuQFwgLhAXIC4QF4gL\
        EBd8ynEcz0tx/fQXAJYLxAXiAsQFW8fllxrw2qtGLBc4FoK4oP2R8K24fHeB5YJ8cVkvuN6E5QLH\
        QsizWpficjSEhcslMKzW+w04FsIu31zWC6tluSDXclkvrNbi5RIYwnIshK+H9eu4rBcsXC6BYbUW\
        HgsFhrAWfnMJDGEtiktgCGthXAJDWAvjEhjCWhiXwOge1tK4BEbnsJbHJTC6hjXGGI8551f+IOd5\
        iowWUX09LpHRKayvHAsdE+kY1i3LZcXo8kKPjn9ohFV+uawYlV/c4WIgrOLLZcWo9oLeMi6RUeHU\
        s3VcIiPzp0SKuERGxu/zVHGJTFSZft6UcYlMVOISGc2CKheX0EQlLqFRPKg2cQlNUOISmqDEJTbE\
        JC6xiUlcYhOSuGganZDEJT7xiIs1UYpEXNCC/ycyiAvEBYgLxAXp/AXjuiZ5GU6sTAAAAABJRU5E\
        rkJggg==")

        resources.hac.bottleLine = decodeBuffer("\
        iVBORw0KGgoAAAANSUhEUgAAANcAAAEoCAYAAADLzmTNAAAC4HpUWHRSYXcgcHJvZmlsZSB0eXBl\
        IGV4aWYAAHja7ZZJkt0gDIb3nCJHQBJCcBzGqtwgx88Ppt+UTqoyLLJ40DZYliWhT/DajW9fp/uC\
        RtGiC2op5hg9Wsghc8Ek+atdI/mw77t1O+/oWe44nSljFIxyvYjj6BfI9f6BhSOvz3Jn7Zpsg3J/\
        8WFQlmdeoZwgjyHhS07n2WW+JiU+LOdcYtvETfn1ORiS0RVCYcdDSPy+8+VJrqvgUtxJIhRJAuaC\
        N0tCP+bPraeaP0/gbfaSP9+OXO7pcFdmj0J8ydORk36ev52lx4iIb575MSKTzeXeHvI3Z09zjmt1\
        JaCOcohnUR9L2TMoVqRT9mcR3XAp5rZ7Rk+++IbEdyy1Ol/xkImR8UmBOhWaNPbYqCHEwIMNI3Nj\
        2bIkxpmbXAjQabI5ydIlgUYDOYGYb7HQ9puXPzhL8NwJmkwwBsbP3b0K/rQ/GZpzlTmRP9sGZYG4\
        eNUXwljk1h1aAELz5FR3fsldg39tC6yAoO40Jyyw+HqZqEr32pLNGXQdVIO/Sp6sHwNIEXwrgkEV\
        B/KRRCmSN2YjQh4T+BREzhK4ggCpU+6IkoNgJxgnXr7xjdHWZeVLjOMFIFSiGNBkKYAVgoaI/ZZQ\
        QsWpaFDVqKZJs5YoMUSNEWfVOqeKiQVTi2aWLFtJkkLSFJOllHIqmbPgGFOXY7accs6lwGkJBbYK\
        9AsElavUULXGajXVXEtD+bTQtMVmLbXcSucuHUeA67FbTz33MmiglEYYOuKwkUYeZaLWpswwdcZp\
        M808y40anW37RO2V3K+p0aHGG9TSszs1iM0+TNA6TnQxAzEOBOK2CKCgeTHziULgRW4x85nFiSgj\
        Sl1wOi1iIBgGsU66sbuT+yk3h+z+Ljf+jJxb6P4FObfQPZD7kdsn1HrZx61sQGsXIqc4IQXbD0qF\
        E/7wc/Jno/N/aeBt6G3obeht6G3o/zEkE78a+IfefQeiFRRW+IUizQAAAYVpQ0NQSUNDIHByb2Zp\
        bGUAAHicfZE9SMNAHMVfU0tFKioWEXHIUJ2siIo4ShWLYKG0FVp1MLn0C5o0JCkujoJrwcGPxaqD\
        i7OuDq6CIPgB4uTopOgiJf4vKbSI9eC4H+/uPe7eAUKtxFSzYwJQNctIRCNiOrMq+l/hQy/6MI4B\
        iZl6LLmYQtvxdQ8PX+/CPKv9uT9Ht5I1GeARieeYbljEG8Qzm5bOeZ84yAqSQnxOPGbQBYkfuS67\
        /MY577DAM4NGKjFPHCQW8y0stzArGCrxNHFIUTXKF9IuK5y3OKulCmvck78wkNVWklynOYwolhBD\
        HCJkVFBECRbCtGqkmEjQfqSNf8jxx8klk6sIRo4FlKFCcvzgf/C7WzM3NekmBSKA78W2P0YA/y5Q\
        r9r297Ft108A7zNwpTX95Row+0l6tamFjoCebeDiuqnJe8DlDjD4pEuG5EhemkIuB7yf0TdlgP5b\
        oGvN7a2xj9MHIEVdLd8AB4fAaJ6y19u8u7O1t3/PNPr7AZMacrSjaRzjAAATM2lUWHRYTUw6Y29t\
        LmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6\
        TlRjemtjOWQiPz4KPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0i\
        WE1QIENvcmUgNC40LjAtRXhpdjIiPgogPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3Lncz\
        Lm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICA8cmRmOkRlc2NyaXB0aW9uIHJkZjph\
        Ym91dD0iIgogICAgeG1sbnM6aXB0Y0V4dD0iaHR0cDovL2lwdGMub3JnL3N0ZC9JcHRjNHhtcEV4\
        dC8yMDA4LTAyLTI5LyIKICAgIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8x\
        LjAvbW0vIgogICAgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlw\
        ZS9SZXNvdXJjZUV2ZW50IyIKICAgIHhtbG5zOnBsdXM9Imh0dHA6Ly9ucy51c2VwbHVzLm9yZy9s\
        ZGYveG1wLzEuMC8iCiAgICB4bWxuczpHSU1QPSJodHRwOi8vd3d3LmdpbXAub3JnL3htcC8iCiAg\
        ICB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iCiAgICB4bWxuczpl\
        eGlmPSJodHRwOi8vbnMuYWRvYmUuY29tL2V4aWYvMS4wLyIKICAgIHhtbG5zOnBob3Rvc2hvcD0i\
        aHR0cDovL25zLmFkb2JlLmNvbS9waG90b3Nob3AvMS4wLyIKICAgIHhtbG5zOnRpZmY9Imh0dHA6\
        Ly9ucy5hZG9iZS5jb20vdGlmZi8xLjAvIgogICAgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUu\
        Y29tL3hhcC8xLjAvIgogICB4bXBNTTpEb2N1bWVudElEPSJhZG9iZTpkb2NpZDpwaG90b3Nob3A6\
        OWE4NGRhNGEtZjlmNi0wYjQwLTg3M2EtOTU4NDdmZGMxMzc0IgogICB4bXBNTTpJbnN0YW5jZUlE\
        PSJ4bXAuaWlkOjQ1NjFjMWI5LTdlNTctNGY0Yi1iNjczLWZmZDRiNzE5YmNlZiIKICAgeG1wTU06\
        T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlkOmNkYjc2Y2VkLTJhYTMtZmY0ZC05ODIwLWJlZDRm\
        Mjg2NjBjMCIKICAgR0lNUDpBUEk9IjIuMCIKICAgR0lNUDpQbGF0Zm9ybT0iV2luZG93cyIKICAg\
        R0lNUDpUaW1lU3RhbXA9IjE1ODk2NTM5MzEwNjc3MDMiCiAgIEdJTVA6VmVyc2lvbj0iMi4xMC4x\
        MiIKICAgZGM6Rm9ybWF0PSJpbWFnZS9wbmciCiAgIGV4aWY6Q29sb3JTcGFjZT0iNjU1MzUiCiAg\
        IGV4aWY6UGl4ZWxYRGltZW5zaW9uPSIyMTUiCiAgIGV4aWY6UGl4ZWxZRGltZW5zaW9uPSIyOTYi\
        CiAgIHBob3Rvc2hvcDpDb2xvck1vZGU9IjMiCiAgIHRpZmY6T3JpZW50YXRpb249IjEiCiAgIHRp\
        ZmY6UmVzb2x1dGlvblVuaXQ9IjIiCiAgIHRpZmY6WFJlc29sdXRpb249IjcyMDAwMC8xMDAwMCIK\
        ICAgdGlmZjpZUmVzb2x1dGlvbj0iNzIwMDAwLzEwMDAwIgogICB4bXA6Q3JlYXRlRGF0ZT0iMjAy\
        MC0wNS0xNFQxMToyMDoyNSswMTowMCIKICAgeG1wOkNyZWF0b3JUb29sPSJHSU1QIDIuMTAiCiAg\
        IHhtcDpNZXRhZGF0YURhdGU9IjIwMjAtMDUtMTRUMTE6MzM6MzMrMDE6MDAiCiAgIHhtcDpNb2Rp\
        ZnlEYXRlPSIyMDIwLTA1LTE0VDExOjMzOjMzKzAxOjAwIj4KICAgPGlwdGNFeHQ6TG9jYXRpb25D\
        cmVhdGVkPgogICAgPHJkZjpCYWcvPgogICA8L2lwdGNFeHQ6TG9jYXRpb25DcmVhdGVkPgogICA8\
        aXB0Y0V4dDpMb2NhdGlvblNob3duPgogICAgPHJkZjpCYWcvPgogICA8L2lwdGNFeHQ6TG9jYXRp\
        b25TaG93bj4KICAgPGlwdGNFeHQ6QXJ0d29ya09yT2JqZWN0PgogICAgPHJkZjpCYWcvPgogICA8\
        L2lwdGNFeHQ6QXJ0d29ya09yT2JqZWN0PgogICA8aXB0Y0V4dDpSZWdpc3RyeUlkPgogICAgPHJk\
        ZjpCYWcvPgogICA8L2lwdGNFeHQ6UmVnaXN0cnlJZD4KICAgPHhtcE1NOkhpc3Rvcnk+CiAgICA8\
        cmRmOlNlcT4KICAgICA8cmRmOmxpCiAgICAgIHN0RXZ0OmFjdGlvbj0iY3JlYXRlZCIKICAgICAg\
        c3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDpjZGI3NmNlZC0yYWEzLWZmNGQtOTgyMC1iZWQ0ZjI4\
        NjYwYzAiCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rvc2hvcCAyMS4wIChX\
        aW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMC0wNS0xNFQxMToyMDoyNSswMTowMCIvPgog\
        ICAgIDxyZGY6bGkKICAgICAgc3RFdnQ6YWN0aW9uPSJzYXZlZCIKICAgICAgc3RFdnQ6Y2hhbmdl\
        ZD0iLyIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDphYzE2MDM0Yy1kMzhjLTExNDUt\
        YmIxNC1iMGYzYzdkMDY3ZDciCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rv\
        c2hvcCAyMS4wIChXaW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMC0wNS0xNFQxMTozMzoz\
        MyswMTowMCIvPgogICAgIDxyZGY6bGkKICAgICAgc3RFdnQ6YWN0aW9uPSJzYXZlZCIKICAgICAg\
        c3RFdnQ6Y2hhbmdlZD0iLyIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo2OGIzYTJl\
        Zi0yNzk4LTQwOGItYmI3Yi1jODMxOTAyMTlkOWYiCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9\
        IkdpbXAgMi4xMCAoV2luZG93cykiCiAgICAgIHN0RXZ0OndoZW49IjIwMjAtMDUtMTZUMTQ6MzI6\
        MTEiLz4KICAgIDwvcmRmOlNlcT4KICAgPC94bXBNTTpIaXN0b3J5PgogICA8cGx1czpJbWFnZVN1\
        cHBsaWVyPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6SW1hZ2VTdXBwbGllcj4KICAgPHBsdXM6\
        SW1hZ2VDcmVhdG9yPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6SW1hZ2VDcmVhdG9yPgogICA8\
        cGx1czpDb3B5cmlnaHRPd25lcj4KICAgIDxyZGY6U2VxLz4KICAgPC9wbHVzOkNvcHlyaWdodE93\
        bmVyPgogICA8cGx1czpMaWNlbnNvcj4KICAgIDxyZGY6U2VxLz4KICAgPC9wbHVzOkxpY2Vuc29y\
        PgogIDwvcmRmOkRlc2NyaXB0aW9uPgogPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgIAo8P3hwYWNrZXQgZW5k\
        PSJ3Ij8+dYDPBgAAAAZiS0dEAPUA9QD1B0/T+gAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1F\
        B+QFEBIgC8QMoe8AABYXSURBVHja7Z3tdds4E4Xv+uz/5VYQuoIwFZiuIHIFkSuwXIGlCiRXIKUC\
        ayswXUGUCsxUYL0V6P1B8JhRZHFAgiA+7nOOjp1YliUAFzMYDAZ/HQ4HEELMc8EmIITiIsQr/mYT\
        GCUBkJ34HgA+q/9rkqrHUJTqcfx/vxr/3gHYn/ie9OQvrrm0yI++Xqmv2Qnh+MxeCQ0AXtTX4uj/\
        CcXVWUS1VbmyYGF8o7aIL43vCzYLxXVMph5Xje9JN3bq8dL4nuKKTEy5ElMemDvnontZKLEVsYkt\
        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")

        resources.hac.cloudCircle = decodeBuffer("\
        iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAAACXBIWXMAAAsTAAALEwEAmpwYAAAA\
        B3RJTUUH5AULBgwjVL+5iQAAABZ0RVh0RmlsZSBOYW1lAHBzY2lyY2xlLnBuZ2utCN4AADucaVRY\
        dFhNTDpjb20uYWRvYmUueG1wAAAAAAA8P3hwYWNrZXQgYmVnaW49Iu+7vyIgaWQ9Ilc1TTBNcENl\
        aGlIenJlU3pOVGN6a2M5ZCI/Pgo8eDp4bXBtZXRhIHhtbG5zOng9ImFkb2JlOm5zOm1ldGEvIiB4\
        OnhtcHRrPSJBZG9iZSBYTVAgQ29yZSA1LjYtYzE0OCA3OS4xNjQwMzYsIDIwMTkvMDgvMTMtMDE6\
        MDY6NTcgICAgICAgICI+CiAgIDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcv\
        MTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+CiAgICAgIDxyZGY6RGVzY3JpcHRpb24gcmRmOmFi\
        b3V0PSIiCiAgICAgICAgICAgIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4w\
        LyIKICAgICAgICAgICAgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9t\
        bS8iCiAgICAgICAgICAgIHhtbG5zOnN0RXZ0PSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAv\
        c1R5cGUvUmVzb3VyY2VFdmVudCMiCiAgICAgICAgICAgIHhtbG5zOmRjPSJodHRwOi8vcHVybC5v\
        cmcvZGMvZWxlbWVudHMvMS4xLyIKICAgICAgICAgICAgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8v\
        bnMuYWRvYmUuY29tL3Bob3Rvc2hvcC8xLjAvIgogICAgICAgICAgICB4bWxuczp0aWZmPSJodHRw\
        Oi8vbnMuYWRvYmUuY29tL3RpZmYvMS4wLyIKICAgICAgICAgICAgeG1sbnM6ZXhpZj0iaHR0cDov\
        L25zLmFkb2JlLmNvbS9leGlmLzEuMC8iPgogICAgICAgICA8eG1wOkNyZWF0b3JUb29sPkFkb2Jl\
        IFBob3Rvc2hvcCAyMS4wIChXaW5kb3dzKTwveG1wOkNyZWF0b3JUb29sPgogICAgICAgICA8eG1w\
        OkNyZWF0ZURhdGU+MjAyMC0wNS0wOFQwODowNDo1NyswMTowMDwveG1wOkNyZWF0ZURhdGU+CiAg\
        ICAgICAgIDx4bXA6TWV0YWRhdGFEYXRlPjIwMjAtMDUtMTFUMDc6MTI6MzIrMDE6MDA8L3htcDpN\
        ZXRhZGF0YURhdGU+CiAgICAgICAgIDx4bXA6TW9kaWZ5RGF0ZT4yMDIwLTA1LTExVDA3OjEyOjMy\
        KzAxOjAwPC94bXA6TW9kaWZ5RGF0ZT4KICAgICAgICAgPHhtcE1NOkluc3RhbmNlSUQ+eG1wLmlp\
        ZDpjOGM5YWZiYS0wZGI3LTdhNGYtODYzMi01NDMyZGI1NTJkZjI8L3htcE1NOkluc3RhbmNlSUQ+\
        CiAgICAgICAgIDx4bXBNTTpEb2N1bWVudElEPmFkb2JlOmRvY2lkOnBob3Rvc2hvcDoyMmY1ZDFj\
        ZS1hN2NkLTBjNGUtOWZjMi1iZjJhYjY4MWRjN2E8L3htcE1NOkRvY3VtZW50SUQ+CiAgICAgICAg\
        IDx4bXBNTTpPcmlnaW5hbERvY3VtZW50SUQ+eG1wLmRpZDo4YmJmMDNmZC1iNWFlLTdhNGMtYjZh\
        YS1lOGE4YjAwNmVlZmY8L3htcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD4KICAgICAgICAgPHhtcE1N\
        Okhpc3Rvcnk+CiAgICAgICAgICAgIDxyZGY6U2VxPgogICAgICAgICAgICAgICA8cmRmOmxpIHJk\
        ZjpwYXJzZVR5cGU9IlJlc291cmNlIj4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OmFjdGlvbj5j\
        cmVhdGVkPC9zdEV2dDphY3Rpb24+CiAgICAgICAgICAgICAgICAgIDxzdEV2dDppbnN0YW5jZUlE\
        PnhtcC5paWQ6OGJiZjAzZmQtYjVhZS03YTRjLWI2YWEtZThhOGIwMDZlZWZmPC9zdEV2dDppbnN0\
        YW5jZUlEPgogICAgICAgICAgICAgICAgICA8c3RFdnQ6d2hlbj4yMDIwLTA1LTA4VDA4OjA0OjU3\
        KzAxOjAwPC9zdEV2dDp3aGVuPgogICAgICAgICAgICAgICAgICA8c3RFdnQ6c29mdHdhcmVBZ2Vu\
        dD5BZG9iZSBQaG90b3Nob3AgMjEuMCAoV2luZG93cyk8L3N0RXZ0OnNvZnR3YXJlQWdlbnQ+CiAg\
        ICAgICAgICAgICAgIDwvcmRmOmxpPgogICAgICAgICAgICAgICA8cmRmOmxpIHJkZjpwYXJzZVR5\
        cGU9IlJlc291cmNlIj4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OmFjdGlvbj5zYXZlZDwvc3RF\
        dnQ6YWN0aW9uPgogICAgICAgICAgICAgICAgICA8c3RFdnQ6aW5zdGFuY2VJRD54bXAuaWlkOjgx\
        OTI4ZmI4LTNlNWMtY2E0NC1hYzdjLWRjNTliNDA4ZGI3ZDwvc3RFdnQ6aW5zdGFuY2VJRD4KICAg\
        ICAgICAgICAgICAgICAgPHN0RXZ0OndoZW4+MjAyMC0wNS0wOFQwODowNDo1NyswMTowMDwvc3RF\
        dnQ6d2hlbj4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OnNvZnR3YXJlQWdlbnQ+QWRvYmUgUGhv\
        dG9zaG9wIDIxLjAgKFdpbmRvd3MpPC9zdEV2dDpzb2Z0d2FyZUFnZW50PgogICAgICAgICAgICAg\
        ICAgICA8c3RFdnQ6Y2hhbmdlZD4vPC9zdEV2dDpjaGFuZ2VkPgogICAgICAgICAgICAgICA8L3Jk\
        ZjpsaT4KICAgICAgICAgICAgICAgPHJkZjpsaSByZGY6cGFyc2VUeXBlPSJSZXNvdXJjZSI+CiAg\
        ICAgICAgICAgICAgICAgIDxzdEV2dDphY3Rpb24+c2F2ZWQ8L3N0RXZ0OmFjdGlvbj4KICAgICAg\
        ICAgICAgICAgICAgPHN0RXZ0Omluc3RhbmNlSUQ+eG1wLmlpZDpjOGM5YWZiYS0wZGI3LTdhNGYt\
        ODYzMi01NDMyZGI1NTJkZjI8L3N0RXZ0Omluc3RhbmNlSUQ+CiAgICAgICAgICAgICAgICAgIDxz\
        dEV2dDp3aGVuPjIwMjAtMDUtMTFUMDc6MTI6MzIrMDE6MDA8L3N0RXZ0OndoZW4+CiAgICAgICAg\
        ICAgICAgICAgIDxzdEV2dDpzb2Z0d2FyZUFnZW50PkFkb2JlIFBob3Rvc2hvcCAyMS4wIChXaW5k\
        b3dzKTwvc3RFdnQ6c29mdHdhcmVBZ2VudD4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OmNoYW5n\
        ZWQ+Lzwvc3RFdnQ6Y2hhbmdlZD4KICAgICAgICAgICAgICAgPC9yZGY6bGk+CiAgICAgICAgICAg\
        IDwvcmRmOlNlcT4KICAgICAgICAgPC94bXBNTTpIaXN0b3J5PgogICAgICAgICA8ZGM6Zm9ybWF0\
        PmltYWdlL3BuZzwvZGM6Zm9ybWF0PgogICAgICAgICA8cGhvdG9zaG9wOkNvbG9yTW9kZT4zPC9w\
        aG90b3Nob3A6Q29sb3JNb2RlPgogICAgICAgICA8dGlmZjpPcmllbnRhdGlvbj4xPC90aWZmOk9y\
        aWVudGF0aW9uPgogICAgICAgICA8dGlmZjpYUmVzb2x1dGlvbj43MjAwMDAvMTAwMDA8L3RpZmY6\
        WFJlc29sdXRpb24+CiAgICAgICAgIDx0aWZmOllSZXNvbHV0aW9uPjcyMDAwMC8xMDAwMDwvdGlm\
        ZjpZUmVzb2x1dGlvbj4KICAgICAgICAgPHRpZmY6UmVzb2x1dGlvblVuaXQ+MjwvdGlmZjpSZXNv\
        bHV0aW9uVW5pdD4KICAgICAgICAgPGV4aWY6Q29sb3JTcGFjZT42NTUzNTwvZXhpZjpDb2xvclNw\
        YWNlPgogICAgICAgICA8ZXhpZjpQaXhlbFhEaW1lbnNpb24+MTI4PC9leGlmOlBpeGVsWERpbWVu\
        c2lvbj4KICAgICAgICAgPGV4aWY6UGl4ZWxZRGltZW5zaW9uPjEyODwvZXhpZjpQaXhlbFlEaW1l\
        bnNpb24+CiAgICAgIDwvcmRmOkRlc2NyaXB0aW9uPgogICA8L3JkZjpSREY+CjwveDp4bXBtZXRh\
        PgogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAo8P3hwYWNrZXQgZW5kPSJ3Ij8+esLpUAAAABh0RVh0U29m\
        dHdhcmUAQWRvYmUgUGhvdG9zaG9wNMuOZwAAAA90RVh0V3JpdGVyAFN1cGVyUE5HxcSv3QAAB5NJ\
        REFUeNrt3Vuw1WUZx/HPprDYNSVHp0KHyILIEKaiAbmKG5zsJlE6GNlUxgUZ46ELJ4kac6bCptCZ\
        dBhjUpsIsCmL5KLxSq3MRilQMkCiXY4cBCeDgonVxfvfu+VmH9baex3+h+c7sy/2rLXX+u/n91vP\
        +6z3/77P21Or1QTV5bUl/b8uxDy8A7NwES7A1OxnEibijdnzX8EZnMKx7OdFHMJB7Mcz+FvZAtVT\
        ggwwBUuxBB/EpZjcpvc6jl34HR7Ho3gpDNBZJmRCX579vA89XbqWs/gDduLhzBhnwwDtYTFWYgXe\
        ltNr/Du2Y2uWIcIA42Q6PoXPY27BMtVebML9OBIGaI45uAGr8PqC1yj/xn34Lp4NA4zMfHwdH8nG\
        +jJRw0NYhz+GAV7NxbgdV5ZQ+KGMsB23YF/VDXA+voIv4jzV4jTuxG04UUUDfAzfwwzV5jC+hC1V\
        McBM3I0PC+rZgdXoK7MBVuAeafYuOJfjuC6rETpCpwquSbgX20L8EZmcxehe9JYlA7wdP8WC0Lcp\
        nsZH8XyRM8AyPBnij4kFWeyWFdUA1+JXkfLHxRTpJtO1RTPArdhcwe/27WBiFstbi1AD9OCbuDl0\
        awsb8GVpNjF3BujBRqwJndrKXbi+VSZo5RDwrRC/I6zBt/NWA6zDTaFNx7gRX83LEPBJadFDT+jS\
        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\
        7pPwLBpu1mUAAAAASUVORK5CYII=")

        resources.hac.heart = decodeBuffer("\
        iVBORw0KGgoAAAANSUhEUgAAAG0AAABeCAYAAAApFppVAAAACXBIWXMAAAsTAAALEwEAmpwYAAAA\
        B3RJTUUH5AUOChgzWWJ37gAAABN0RVh0RmlsZSBOYW1lAGhlYXJ0LnBuZ+bGlqYAADnkaVRYdFhN\
        TDpjb20uYWRvYmUueG1wAAAAAAA8P3hwYWNrZXQgYmVnaW49Iu+7vyIgaWQ9Ilc1TTBNcENlaGlI\
        enJlU3pOVGN6a2M5ZCI/Pgo8eDp4bXBtZXRhIHhtbG5zOng9ImFkb2JlOm5zOm1ldGEvIiB4Onht\
        cHRrPSJBZG9iZSBYTVAgQ29yZSA1LjYtYzE0OCA3OS4xNjQwMzYsIDIwMTkvMDgvMTMtMDE6MDY6\
        NTcgICAgICAgICI+CiAgIDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcvMTk5\
        OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+CiAgICAgIDxyZGY6RGVzY3JpcHRpb24gcmRmOmFib3V0\
        PSIiCiAgICAgICAgICAgIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIK\
        ICAgICAgICAgICAgeG1sbnM6ZGM9Imh0dHA6Ly9wdXJsLm9yZy9kYy9lbGVtZW50cy8xLjEvIgog\
        ICAgICAgICAgICB4bWxuczpwaG90b3Nob3A9Imh0dHA6Ly9ucy5hZG9iZS5jb20vcGhvdG9zaG9w\
        LzEuMC8iCiAgICAgICAgICAgIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8x\
        LjAvbW0vIgogICAgICAgICAgICB4bWxuczpzdEV2dD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAv\
        MS4wL3NUeXBlL1Jlc291cmNlRXZlbnQjIgogICAgICAgICAgICB4bWxuczp0aWZmPSJodHRwOi8v\
        bnMuYWRvYmUuY29tL3RpZmYvMS4wLyIKICAgICAgICAgICAgeG1sbnM6ZXhpZj0iaHR0cDovL25z\
        LmFkb2JlLmNvbS9leGlmLzEuMC8iPgogICAgICAgICA8eG1wOkNyZWF0b3JUb29sPkFkb2JlIFBo\
        b3Rvc2hvcCAyMS4wIChXaW5kb3dzKTwveG1wOkNyZWF0b3JUb29sPgogICAgICAgICA8eG1wOkNy\
        ZWF0ZURhdGU+MjAyMC0wNS0xNFQxMToyMDoyNSswMTowMDwveG1wOkNyZWF0ZURhdGU+CiAgICAg\
        ICAgIDx4bXA6TW9kaWZ5RGF0ZT4yMDIwLTA1LTE0VDExOjI0OjQ5KzAxOjAwPC94bXA6TW9kaWZ5\
        RGF0ZT4KICAgICAgICAgPHhtcDpNZXRhZGF0YURhdGU+MjAyMC0wNS0xNFQxMToyNDo0OSswMTow\
        MDwveG1wOk1ldGFkYXRhRGF0ZT4KICAgICAgICAgPGRjOmZvcm1hdD5pbWFnZS9wbmc8L2RjOmZv\
        cm1hdD4KICAgICAgICAgPHBob3Rvc2hvcDpDb2xvck1vZGU+MzwvcGhvdG9zaG9wOkNvbG9yTW9k\
        ZT4KICAgICAgICAgPHhtcE1NOkluc3RhbmNlSUQ+eG1wLmlpZDo1OTcyM2ZhZC00N2ZhLTNiNDAt\
        YmI0My01M2M2OTRiMjQwMDY8L3htcE1NOkluc3RhbmNlSUQ+CiAgICAgICAgIDx4bXBNTTpEb2N1\
        bWVudElEPmFkb2JlOmRvY2lkOnBob3Rvc2hvcDo3NTBiNTU4Ny1iMTVjLTY1NGMtYjE1Ni1iMTAy\
        NGZmOTFlMDU8L3htcE1NOkRvY3VtZW50SUQ+CiAgICAgICAgIDx4bXBNTTpPcmlnaW5hbERvY3Vt\
        ZW50SUQ+eG1wLmRpZDpmYWFhYjI4Ni04ZWI3LTMyNDAtYTJlMC1kNDNjNmU2ZjdlYjc8L3htcE1N\
        Ok9yaWdpbmFsRG9jdW1lbnRJRD4KICAgICAgICAgPHhtcE1NOkhpc3Rvcnk+CiAgICAgICAgICAg\
        IDxyZGY6U2VxPgogICAgICAgICAgICAgICA8cmRmOmxpIHJkZjpwYXJzZVR5cGU9IlJlc291cmNl\
        Ij4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OmFjdGlvbj5jcmVhdGVkPC9zdEV2dDphY3Rpb24+\
        CiAgICAgICAgICAgICAgICAgIDxzdEV2dDppbnN0YW5jZUlEPnhtcC5paWQ6ZmFhYWIyODYtOGVi\
        Ny0zMjQwLWEyZTAtZDQzYzZlNmY3ZWI3PC9zdEV2dDppbnN0YW5jZUlEPgogICAgICAgICAgICAg\
        ICAgICA8c3RFdnQ6d2hlbj4yMDIwLTA1LTE0VDExOjIwOjI1KzAxOjAwPC9zdEV2dDp3aGVuPgog\
        ICAgICAgICAgICAgICAgICA8c3RFdnQ6c29mdHdhcmVBZ2VudD5BZG9iZSBQaG90b3Nob3AgMjEu\
        MCAoV2luZG93cyk8L3N0RXZ0OnNvZnR3YXJlQWdlbnQ+CiAgICAgICAgICAgICAgIDwvcmRmOmxp\
        PgogICAgICAgICAgICAgICA8cmRmOmxpIHJkZjpwYXJzZVR5cGU9IlJlc291cmNlIj4KICAgICAg\
        ICAgICAgICAgICAgPHN0RXZ0OmFjdGlvbj5zYXZlZDwvc3RFdnQ6YWN0aW9uPgogICAgICAgICAg\
        ICAgICAgICA8c3RFdnQ6aW5zdGFuY2VJRD54bXAuaWlkOjU5NzIzZmFkLTQ3ZmEtM2I0MC1iYjQz\
        LTUzYzY5NGIyNDAwNjwvc3RFdnQ6aW5zdGFuY2VJRD4KICAgICAgICAgICAgICAgICAgPHN0RXZ0\
        OndoZW4+MjAyMC0wNS0xNFQxMToyNDo0OSswMTowMDwvc3RFdnQ6d2hlbj4KICAgICAgICAgICAg\
        ICAgICAgPHN0RXZ0OnNvZnR3YXJlQWdlbnQ+QWRvYmUgUGhvdG9zaG9wIDIxLjAgKFdpbmRvd3Mp\
        PC9zdEV2dDpzb2Z0d2FyZUFnZW50PgogICAgICAgICAgICAgICAgICA8c3RFdnQ6Y2hhbmdlZD4v\
        PC9zdEV2dDpjaGFuZ2VkPgogICAgICAgICAgICAgICA8L3JkZjpsaT4KICAgICAgICAgICAgPC9y\
        ZGY6U2VxPgogICAgICAgICA8L3htcE1NOkhpc3Rvcnk+CiAgICAgICAgIDx0aWZmOk9yaWVudGF0\
        aW9uPjE8L3RpZmY6T3JpZW50YXRpb24+CiAgICAgICAgIDx0aWZmOlhSZXNvbHV0aW9uPjcyMDAw\
        MC8xMDAwMDwvdGlmZjpYUmVzb2x1dGlvbj4KICAgICAgICAgPHRpZmY6WVJlc29sdXRpb24+NzIw\
        MDAwLzEwMDAwPC90aWZmOllSZXNvbHV0aW9uPgogICAgICAgICA8dGlmZjpSZXNvbHV0aW9uVW5p\
        dD4yPC90aWZmOlJlc29sdXRpb25Vbml0PgogICAgICAgICA8ZXhpZjpDb2xvclNwYWNlPjY1NTM1\
        PC9leGlmOkNvbG9yU3BhY2U+CiAgICAgICAgIDxleGlmOlBpeGVsWERpbWVuc2lvbj4xMDk8L2V4\
        aWY6UGl4ZWxYRGltZW5zaW9uPgogICAgICAgICA8ZXhpZjpQaXhlbFlEaW1lbnNpb24+OTQ8L2V4\
        aWY6UGl4ZWxZRGltZW5zaW9uPgogICAgICA8L3JkZjpEZXNjcmlwdGlvbj4KICAgPC9yZGY6UkRG\
        Pgo8L3g6eG1wbWV0YT4KICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAKPD94cGFja2V0IGVuZD0idyI/Pl5S\
        RLAAAAAYdEVYdFNvZnR3YXJlAEFkb2JlIFBob3Rvc2hvcDTLjmcAAAAPdEVYdFdyaXRlcgBTdXBl\
        clBOR8XEr90AAASMSURBVHja7Z3tcZtAEIZfMSmADiJXEFxBpAoiVWBUgeUKLFVguQKcCqxUIFyB\
        lQpMB6aDzQ8OB2PEl+6OPbTvjEYejwdz+7B7u8exTIgIIrf0TUzQWT6AQH18AD/Ud64X9R0DOAJI\
        dZ/AZABPywedqkG5oADADYCZ+rmLjgrgI4CEOzQfwEJdiYEa8CnlAF8Kg0wHBjUFcKvGMNV0zBjA\
        Vn2zghYWrspztAfwR33bBDgrwDKlPYBV73ERkY6PT0QbInonM4qIKNB0rqc+AREdyJ7eiWjR51x1\
        DHZBRG+WBnogoplmWL66KIbSxiY0n4ieBxpoRERTDcDWBqND1/G0Pu++c1oA4FnjBN1HqcrINj2T\
        jEjDvKtTT2qeMzKnLZhcnbleO853IbPzL2ptwtNCdYVy1B2AXUMJEhnOCnVo3lQSdIHGGVhTKh2o\
        cw/AXymAq7pywGt5oIUDwPLzPJTgVP2O+zJZdK6nBWrQvkPrgymAZSHhcFFLFTk6Q/MBvA6cJV6q\
        EhUmO4fHSIANuva56eppawAPYjt+SYlXQ/lebMYiKVm39bQDs9UCmdsaPG0mwNjNbWETtEjsxE43\
        ddBCyRZZalbk4tURFbHSograVOYy1vpVBe1W7MI+RH6BJl7mCDivEBoDsYlb0ASYG/ou0NwstD+g\
        /RR7OKGgqk4T8ZYP/F8wlued3NFEPM1BCTSBJrIJLRVTuAftKKZwQomER8ehvYg93IMm4dEN/RVo\
        7ukIfN5C9wbZH8Jdk3IishebsNa+qrj+LXZhrY9ksbzDWEIkX12dqtPE23gqRqFFUxnaTuzDUp+c\
        qQwtRdYaQcRHX5hULWNtxU6s9Fj+RRW0RLyNlZft2kADsp4ccruGh5elbaGlEiZ5elkdtDyTlDVJ\
        Zl5WVVyXFSBrSSGyqwQn2lE0eRqUp0mYtK/abnRte2PJg/P29KQLmo9sXdIXmxpPPq6aMnevw8GW\
        YlPjWrYptbps7IlV/SYyox1atn7v0w43QqmvhehsHQFct/3jvj2MJTGxPI/1DY/l2CuFtx7N0XHJ\
        0Dvj6pgLOC31WGcbeme6tYDrrzv0vJui410zPtzqEexEAW3K08TjBgCmC5qAswhMJzQBZwmYbmg5\
        uGvIdgVjwExAK6aysh3PADBd2WOdQlx2p1btwEx6WvGkO1f8I6rDViYObOvtu1Nk71u7lFpuZXJe\
        t/XMdaI8buwJipVEzLM8oBXGe08uv71ivOQZorvBTg0uGVnCMbc1Jm/gq3IMT5/mCYe1ZMtWItJU\
        FjzAvU1D+b6Z2PY/5gAtzy65vQ23TjFabsIZU3g8lV26sDF2O3TtycXTiuL60tUETLZZcOyNlScp\
        W/BZSdnZSudd9TROc12iMsOYk1G4d6HL57rVAF6Xe1fMzSjcPa0oX5UG4SV6l0ueVq6LVsrzYkPH\
        3yLbOBpzNoRLnmayKN+rlY3EhYG7DC0PmWtkrxHrA499KBwjtGKWed9hvstDoZNbIsYCrS28FNkD\
        6Ds4fDd9bNDq4G1dhzV2aEV4IbL7XclYBjV2aKOU9OV3UP8AtCBKk6XkDkcAAAAASUVORK5CYII=")

        resources.hac.noGame = decodeBuffer("\
        iVBORw0KGgoAAAANSUhEUgAAAkAAAABaCAYAAABQSOniAAAACXBIWXMAAAsTAAALEwEAmpwYAAAA\
        B3RJTUUH5AUOChoo4THcgAAAABR0RVh0RmlsZSBOYW1lAG5vZ2FtZS5wbmfRvNmZAAA55GlUWHRY\
        TUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhp\
        SHpyZVN6TlRjemtjOWQiPz4KPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4\
        bXB0az0iQWRvYmUgWE1QIENvcmUgNS42LWMxNDggNzkuMTY0MDM2LCAyMDE5LzA4LzEzLTAxOjA2\
        OjU3ICAgICAgICAiPgogICA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5\
        OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPgogICAgICA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91\
        dD0iIgogICAgICAgICAgICB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8i\
        CiAgICAgICAgICAgIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIK\
        ICAgICAgICAgICAgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8vbnMuYWRvYmUuY29tL3Bob3Rvc2hv\
        cC8xLjAvIgogICAgICAgICAgICB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAv\
        MS4wL21tLyIKICAgICAgICAgICAgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFw\
        LzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIKICAgICAgICAgICAgeG1sbnM6dGlmZj0iaHR0cDov\
        L25zLmFkb2JlLmNvbS90aWZmLzEuMC8iCiAgICAgICAgICAgIHhtbG5zOmV4aWY9Imh0dHA6Ly9u\
        cy5hZG9iZS5jb20vZXhpZi8xLjAvIj4KICAgICAgICAgPHhtcDpDcmVhdG9yVG9vbD5BZG9iZSBQ\
        aG90b3Nob3AgMjEuMCAoV2luZG93cyk8L3htcDpDcmVhdG9yVG9vbD4KICAgICAgICAgPHhtcDpD\
        cmVhdGVEYXRlPjIwMjAtMDUtMTRUMTE6MjA6MjUrMDE6MDA8L3htcDpDcmVhdGVEYXRlPgogICAg\
        ICAgICA8eG1wOk1vZGlmeURhdGU+MjAyMC0wNS0xNFQxMToyNjozOCswMTowMDwveG1wOk1vZGlm\
        eURhdGU+CiAgICAgICAgIDx4bXA6TWV0YWRhdGFEYXRlPjIwMjAtMDUtMTRUMTE6MjY6MzgrMDE6\
        MDA8L3htcDpNZXRhZGF0YURhdGU+CiAgICAgICAgIDxkYzpmb3JtYXQ+aW1hZ2UvcG5nPC9kYzpm\
        b3JtYXQ+CiAgICAgICAgIDxwaG90b3Nob3A6Q29sb3JNb2RlPjM8L3Bob3Rvc2hvcDpDb2xvck1v\
        ZGU+CiAgICAgICAgIDx4bXBNTTpJbnN0YW5jZUlEPnhtcC5paWQ6MzYwYWIzMDItZTg4MS1iZDQ0\
        LTk4NTctN2IxZTNmMGU2ZWE1PC94bXBNTTpJbnN0YW5jZUlEPgogICAgICAgICA8eG1wTU06RG9j\
        dW1lbnRJRD5hZG9iZTpkb2NpZDpwaG90b3Nob3A6ZmZmZmM3OWItNThiNS1lZDQxLThmN2YtYTJk\
        ZDU2YzEzYzFjPC94bXBNTTpEb2N1bWVudElEPgogICAgICAgICA8eG1wTU06T3JpZ2luYWxEb2N1\
        bWVudElEPnhtcC5kaWQ6ODc5ZmI4NDItMGYzNy1iMjRhLWI5NjYtMTgyNzI1M2JmODMwPC94bXBN\
        TTpPcmlnaW5hbERvY3VtZW50SUQ+CiAgICAgICAgIDx4bXBNTTpIaXN0b3J5PgogICAgICAgICAg\
        ICA8cmRmOlNlcT4KICAgICAgICAgICAgICAgPHJkZjpsaSByZGY6cGFyc2VUeXBlPSJSZXNvdXJj\
        ZSI+CiAgICAgICAgICAgICAgICAgIDxzdEV2dDphY3Rpb24+Y3JlYXRlZDwvc3RFdnQ6YWN0aW9u\
        PgogICAgICAgICAgICAgICAgICA8c3RFdnQ6aW5zdGFuY2VJRD54bXAuaWlkOjg3OWZiODQyLTBm\
        MzctYjI0YS1iOTY2LTE4MjcyNTNiZjgzMDwvc3RFdnQ6aW5zdGFuY2VJRD4KICAgICAgICAgICAg\
        ICAgICAgPHN0RXZ0OndoZW4+MjAyMC0wNS0xNFQxMToyMDoyNSswMTowMDwvc3RFdnQ6d2hlbj4K\
        ICAgICAgICAgICAgICAgICAgPHN0RXZ0OnNvZnR3YXJlQWdlbnQ+QWRvYmUgUGhvdG9zaG9wIDIx\
        LjAgKFdpbmRvd3MpPC9zdEV2dDpzb2Z0d2FyZUFnZW50PgogICAgICAgICAgICAgICA8L3JkZjps\
        aT4KICAgICAgICAgICAgICAgPHJkZjpsaSByZGY6cGFyc2VUeXBlPSJSZXNvdXJjZSI+CiAgICAg\
        ICAgICAgICAgICAgIDxzdEV2dDphY3Rpb24+c2F2ZWQ8L3N0RXZ0OmFjdGlvbj4KICAgICAgICAg\
        ICAgICAgICAgPHN0RXZ0Omluc3RhbmNlSUQ+eG1wLmlpZDozNjBhYjMwMi1lODgxLWJkNDQtOTg1\
        Ny03YjFlM2YwZTZlYTU8L3N0RXZ0Omluc3RhbmNlSUQ+CiAgICAgICAgICAgICAgICAgIDxzdEV2\
        dDp3aGVuPjIwMjAtMDUtMTRUMTE6MjY6MzgrMDE6MDA8L3N0RXZ0OndoZW4+CiAgICAgICAgICAg\
        ICAgICAgIDxzdEV2dDpzb2Z0d2FyZUFnZW50PkFkb2JlIFBob3Rvc2hvcCAyMS4wIChXaW5kb3dz\
        KTwvc3RFdnQ6c29mdHdhcmVBZ2VudD4KICAgICAgICAgICAgICAgICAgPHN0RXZ0OmNoYW5nZWQ+\
        Lzwvc3RFdnQ6Y2hhbmdlZD4KICAgICAgICAgICAgICAgPC9yZGY6bGk+CiAgICAgICAgICAgIDwv\
        cmRmOlNlcT4KICAgICAgICAgPC94bXBNTTpIaXN0b3J5PgogICAgICAgICA8dGlmZjpPcmllbnRh\
        dGlvbj4xPC90aWZmOk9yaWVudGF0aW9uPgogICAgICAgICA8dGlmZjpYUmVzb2x1dGlvbj43MjAw\
        MDAvMTAwMDA8L3RpZmY6WFJlc29sdXRpb24+CiAgICAgICAgIDx0aWZmOllSZXNvbHV0aW9uPjcy\
        MDAwMC8xMDAwMDwvdGlmZjpZUmVzb2x1dGlvbj4KICAgICAgICAgPHRpZmY6UmVzb2x1dGlvblVu\
        aXQ+MjwvdGlmZjpSZXNvbHV0aW9uVW5pdD4KICAgICAgICAgPGV4aWY6Q29sb3JTcGFjZT42NTUz\
        NTwvZXhpZjpDb2xvclNwYWNlPgogICAgICAgICA8ZXhpZjpQaXhlbFhEaW1lbnNpb24+NTc2PC9l\
        eGlmOlBpeGVsWERpbWVuc2lvbj4KICAgICAgICAgPGV4aWY6UGl4ZWxZRGltZW5zaW9uPjkwPC9l\
        eGlmOlBpeGVsWURpbWVuc2lvbj4KICAgICAgPC9yZGY6RGVzY3JpcHRpb24+CiAgIDwvcmRmOlJE\
        Rj4KPC94OnhtcG1ldGE+CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAK\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAog\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        CiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg\
        ICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgCjw/eHBhY2tldCBlbmQ9InciPz7s\
        eVnAAAAAGHRFWHRTb2Z0d2FyZQBBZG9iZSBQaG90b3Nob3A0y45nAAAAD3RFWHRXcml0ZXIAU3Vw\
        ZXJQTkfFxK/dAAAcR0lEQVR42u1d7XXbuBK98dn/0VYQuoJoKwhdgZUKQlcQuwLLFVipwEwFlisw\
        XUGUCsytINoK9H4QeGYUSbggAQIg556jk91EHyAwH3cGg8G73W4HgUAgEAgEginhL5kCgUBwAHMA\
        mfrzvfpzH08AVjJVnZABWAD41JprAKgBbNTcrgFsZaoEAj94N4EM0EwZmkwZlrUsu0DwB9rOOLf4\
        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")
    end

    -- 30log.lua begins
    local function require_30log()
        local next, assert, pairs, type, tostring, setmetatable, baseMt, _instances, _classes, _class = next, assert, pairs, type, tostring, setmetatable, {}, setmetatable({},{__mode = 'k'}), setmetatable({},{__mode = 'k'})
        local function assert_call_from_class(class, method) assert(_classes[class], ('Wrong method call. Expected class:%s.'):format(method)) end; local function assert_call_from_instance(instance, method) assert(_instances[instance], ('Wrong method call. Expected instance:%s.'):format(method)) end
        local function bind(f, v) return function(...) return f(v, ...) end end
        local default_filter = function() return true end
        local function deep_copy(t, dest, aType) t = t or {}; local r = dest or {}; for k,v in pairs(t) do if aType ~= nil and type(v) == aType then r[k] = (type(v) == 'table') and ((_classes[v] or _instances[v]) and v or deep_copy(v)) or v elseif aType == nil then r[k] = (type(v) == 'table') and k~= '__index' and ((_classes[v] or _instances[v]) and v or deep_copy(v)) or v end; end return r end
        local function instantiate(call_init,self,...) assert_call_from_class(self, 'new(...) or class(...)'); local instance = {class = self}; _instances[instance] = tostring(instance); deep_copy(self, instance, 'table')
            instance.__index, instance.__subclasses, instance.__instances, instance.mixins = nil, nil, nil, nil; setmetatable(instance,self); if call_init and self.init then if type(self.init) == 'table' then deep_copy(self.init, instance) else self.init(instance, ...) end end; return instance
        end
        local function extend(self, name, extra_params)
            assert_call_from_class(self, 'extend(...)'); local heir = {}; _classes[heir] = tostring(heir); self.__subclasses[heir] = true; deep_copy(extra_params, deep_copy(self, heir))
            heir.name, heir.__index, heir.super, heir.mixins = extra_params and extra_params.name or name, heir, self, {}; return setmetatable(heir,self)
        end
        baseMt = { __call = function (self,...) return self:new(...) end, __tostring = function(self,...)
            if _instances[self] then return ("instance of '%s' (%s)"):format(rawget(self.class,'name') or '?', _instances[self]) end; return _classes[self] and ("class '%s' (%s)"):format(rawget(self,'name') or '?', _classes[self]) or self end
        }; _classes[baseMt] = tostring(baseMt); setmetatable(baseMt, {__tostring = baseMt.__tostring})
        local class = {isClass = function(t) return not not _classes[t] end, isInstance = function(t) return not not _instances[t] end}
        _class = function(name, attr) local c = deep_copy(attr); _classes[c] = tostring(c)
            c.name, c.__tostring, c.__call, c.new, c.create, c.extend, c.__index, c.mixins, c.__instances, c.__subclasses = name or c.name, baseMt.__tostring, baseMt.__call, bind(instantiate, true), bind(instantiate, false), extend, c, setmetatable({},{__mode = 'k'}), setmetatable({},{__mode = 'k'}), setmetatable({},{__mode = 'k'})
            c.subclasses = function(self, filter, ...) assert_call_from_class(self, 'subclasses(class)'); filter = filter or default_filter; local subclasses = {}; for class in pairs(_classes) do if class ~= baseMt and class:subclassOf(self) and filter(class,...) then subclasses[#subclasses + 1] = class end end; return subclasses end
            c.instances = function(self, filter, ...) assert_call_from_class(self, 'instances(class)'); filter = filter or default_filter; local instances = {}; for instance in pairs(_instances) do if instance:instanceOf(self) and filter(instance, ...) then instances[#instances + 1] = instance end end; return instances end
            c.subclassOf = function(self, superclass) assert_call_from_class(self, 'subclassOf(superclass)'); assert(class.isClass(superclass), 'Wrong argument given to method "subclassOf()". Expected a class.'); local super = self.super; while super do if super == superclass then return true end; super = super.super end; return false end
            c.classOf = function(self, subclass) assert_call_from_class(self, 'classOf(subclass)'); assert(class.isClass(subclass), 'Wrong argument given to method "classOf()". Expected a class.'); return subclass:subclassOf(self) end
            c.instanceOf = function(self, fromclass) assert_call_from_instance(self, 'instanceOf(class)'); assert(class.isClass(fromclass), 'Wrong argument given to method "instanceOf()". Expected a class.'); return ((self.class == fromclass) or (self.class:subclassOf(fromclass))) end
            c.cast = function(self, toclass) assert_call_from_instance(self, 'instanceOf(class)'); assert(class.isClass(toclass), 'Wrong argument given to method "cast()". Expected a class.'); setmetatable(self, toclass); self.class = toclass; return self end
            c.with = function(self,...) assert_call_from_class(self, 'with(mixin)'); for _, mixin in ipairs({...}) do assert(self.mixins[mixin] ~= true, ('Attempted to include a mixin which was already included in %s'):format(tostring(self))); self.mixins[mixin] = true; deep_copy(mixin, self, 'function') end return self end
            c.includes = function(self, mixin) assert_call_from_class(self,'includes(mixin)'); return not not (self.mixins[mixin] or (self.super and self.super:includes(mixin))) end
            c.without = function(self, ...) assert_call_from_class(self, 'without(mixin)'); for _, mixin in ipairs({...}) do
                assert(self.mixins[mixin] == true, ('Attempted to remove a mixin which is not included in %s'):format(tostring(self))); local classes = self:subclasses(); classes[#classes + 1] = self
                for _, class in ipairs(classes) do for method_name, method in pairs(mixin) do if type(method) == 'function' then class[method_name] = nil end end end; self.mixins[mixin] = nil end; return self end; return setmetatable(c, baseMt) end
        class._DESCRIPTION = '30 lines library for object orientation in Lua'; class._VERSION = '30log v1.2.0'; class._URL = 'http://github.com/Yonaba/30log'; class._LICENSE = 'MIT LICENSE <http://www.opensource.org/licenses/mit-license.php>'
        return setmetatable(class,{__call = function(_,...) return _class(...) end })
    end

    local function lerp(a, b, t)
        return a + (b - a) * t
    end

    -- locals --

    local res = {}

    local centerX = 0
    local centerY = 0

    local clouds = {}
    local cloudVerticalPosition = 0

    local timer = 0

    local Cloud  = nil
    local NoGame = nil
    local Bottle = nil
    local Heart  = nil

    local class = require_30log()

    local consoleKey = is3DS and "ctr" or "hac"

    -- Cloud Class --

    Cloud = class("Cloud")
    Cloud.index = 0

    function Cloud:init(x, y, radius)
        self.x = x
        self.y = y

        self.radius = radius or 5

        self.id = Cloud.index

        self.xMove = 0
        self.yMove = 0

        self.xSquash = 0
        self.ySquash = 0

        Cloud.index = Cloud.index + 1
    end

    function Cloud:update()
        self.xMove = math.cos((timer + self.id / 3) * 2) * 10
        self.yMove = math.sin((timer + self.id / 3) * 2) * 10

        self.xSquash = math.abs(math.cos(self.id + timer)) / 5
        self.ySquash = math.abs(math.sin(self.id + (timer / 2))) / 5
    end

    function Cloud:draw(screen, iod)
        if is3DS and screen == "bottom" then
            return
        end

        love.graphics.setColor(0.878, 0.878, 0.878)
        love.graphics.setBlendFactor(1)

        local scale = (self.radius * 2) / res.cloudCircle:getWidth()

        local x, y = self.x + self.xMove, self.y + self.yMove
        local renderScale = scale * (0.8 + self.xSquash)

        local offsetX, offsetY = res.cloudCircle:getWidth() / 2, res.cloudCircle:getHeight() / 2

        love.graphics.draw(res.cloudCircle, x + (iod * (10 + 10 * 0.5)), y, 0, renderScale, renderScale, offsetX, offsetY)
    end

    -- Heart Class --

    Heart = class("Heart")
    Heart.id = 0

    function Heart:init(x, rotation, args)
        self.x = x
        self.y = y

        self.rotation = rotation

        self.thetaOffset = args.thetaOffset or 0
        self.thetaDivisor = args.thetaDivisor or 1

        self.rotationDivisor = args.rotationDivisor or 1

        self.id = Heart.id

        Heart.id = Heart.id + 1
    end

    function Heart:draw(offset, screen, iod)
        if is3DS and screen == "bottom" then
            return
        end

        local theta    = math.cos(0.5 + (timer * 5)) * 0.1
        local rotation = -math.rad(math.cos(timer * 5) * 5)

        if self.id ~= 0 then
            rotation = -rotation
        end

        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.setBlendFactor(0)

        local drawRotation = self.rotation - (rotation / self.rotationDivisor)
        local drawScale = math.abs((self.thetaOffset + theta / self.thetaDivisor))

        local x = self.x - (iod * 6.0)
        love.graphics.draw(res.heart, x, offset, drawRotation, drawScale, drawScale, res.heart:getWidth() / 2, res.heart:getHeight())
    end

    -- Bottle Class --

    Bottle = class("Bottle")

    function Bottle:init(x, y, scale)
        self.x = x
        self.y = y

        self.hearts =
        {
            Heart(self.x - (20 * scale), -math.pi / 7,       { thetaOffset = 0.6 }),
            Heart(self.x + (15 * scale), -math.pi * 3 / 1.6, { thetaOffset = 0.3, thetaDivisor = 2, rotationDivisor = 2 }),
            Heart(self.x + (30 * scale), -math.pi * 3 / 1.6, { thetaOffset = 0.3, thetaDivisor = 1.5 })
        }

        self.heartOffsets = {160 * scale, 140 * scale, 200 * scale}
    end

    function Bottle:draw(screen, iod)
        if is3DS and screen == "bottom" then
            love.graphics.setColor(1, 1, 1, 1)
            return love.graphics.rectangle("fill", 0, 0, love.graphics.getWidth(), love.graphics.getHeight())
        end

        -- draw the cloud height rectangle
        love.graphics.setColor(0.878, 0.878, 0.878)
        love.graphics.setBlendFactor(1)

        love.graphics.rectangle("fill", 0, cloudVerticalPosition, love.graphics.getWidth(), love.graphics.getHeight() - cloudVerticalPosition)

        -- draw the bottle over that

        local y = self.y + math.cos(timer * 5) * 12
        local x = self.x - (iod * 6.0)

        love.graphics.setBlendFactor(0)

        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.draw(res.bottleFill, x, y, 0, 1, 1, res.bottleFill:getWidth() / 2, res.bottleFill:getHeight() / 2)

        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.draw(res.bottleLine, x, y, 0, 1, 1, res.bottleLine:getWidth() / 2, res.bottleLine:getHeight() / 2)

        for index = 1, #self.hearts do
            self.hearts[index]:draw(y - self.heartOffsets[index], screen, iod)
        end
    end

    -- NoGame Class --

    NoGame = class("NoGame")

    function NoGame:init(x, y)
        self.x = x
        self.y = y
    end

    function NoGame:draw(screen)
        if is3DS then
            if screen ~= "bottom" then
                return
            else
                -- draw the "background"
                love.graphics.setColor(0.878, 0.878, 0.878)
                love.graphics.setBlendFactor(1)

                love.graphics.rectangle("fill", 0, 0, love.graphics.getWidth(), love.graphics.getHeight())
            end
        end

        -- draw the text

        local scale = (1 + math.abs(math.cos(1 - (timer * 2.5))) / 20)

        love.graphics.setColor(0.941, 0.384, 0.573)
        love.graphics.setBlendFactor(1)

        love.graphics.draw(res.noGame, self.x, self.y, 0, scale, 1, res.noGame:getWidth() / 2, res.noGame:getHeight() / 2)
    end

    local function generateClouds(cloudSize)
        local cloudCount = math.floor(love.graphics.getWidth() / cloudSize)
        cloudVerticalPosition = love.graphics.getHeight() * 0.75

        -- Create clouds
        clouds = {}
        for i = 0, cloudCount + 1 do
            local nextX, nextY = lerp(0, love.graphics.getWidth(), (i / cloudCount)), cloudVerticalPosition
            local distance = centerX - nextX;

            local newCloud = Cloud(nextX, nextY, (cloudSize / 1.8) * (1 + math.abs(math.cos(i + distance) / 2)))
            table.insert(clouds, newCloud)
        end
    end

    local bottleInstance = nil
    local noGameInstance = nil

    -- debugging info --

    local DEBUG = false
    local rendererInfo = nil

    -- end debugging info --

    function love.load()
        -- nop this function on Switch
        if not is3DS then
            love.graphics.setBlendFactor = function(_)
            end
        end

        love.graphics.setBackgroundColor(0.392, 0.710, 0.965)

        for key, value in pairs(resources[consoleKey]) do
            res[key] = love.graphics.newImage(value)
        end

        local screen = is3DS and "top" or nil

        centerX = love.graphics.getWidth(screen) / 2
        centerY = love.graphics.getHeight() / 2

        local cloudSize = res.cloudCircle:getWidth()
        generateClouds(cloudSize)

        screen = is3DS and "bottom" or nil

        centerX = love.graphics.getWidth(screen) / 2

        local scale = is3DS and 0.5 or 1.0

        local y = love.graphics.getHeight() * 0.50
        if is3DS then
            y = love.graphics.getHeight() * 0.55
        end
        bottleInstance = Bottle(love.graphics.getWidth() / 2, y, scale)

        y = love.graphics.getHeight() * 0.85
        if is3DS then
            y = centerY
        end
        noGameInstance = NoGame(centerX, y)

        -- debug info

        rendererInfo = table.concat({love.graphics.getRendererInfo()}, "\n")

        love.graphics.setNewFont(14)
    end

    function love.update(dt)
        timer = timer + dt

        for _, v in ipairs(clouds) do
            v:update()
        end
    end

    function love.draw(screen)
        if screen ~= "bottom" then
            if DEBUG then
                love.graphics.setColor(0, 0, 0, 0.5)
                local debugString = string.format("FPS: %d\n%s", love.timer.getFPS(), rendererInfo)
                love.graphics.print(debugString)
            end
        end

        local iod = (is3DS and love.graphics.get3DDepth() or 0) / 3

        if screen == "right" then
            iod = -iod
        end

        for _, v in ipairs(clouds) do
            v:draw(screen, -iod)
        end

        bottleInstance:draw(screen, -iod)
        noGameInstance:draw(screen)
    end

    function love.gamepadpressed(joystick, button)
        if button == "start" then
            love.event.quit()
        elseif button == "back" then
            DEBUG = not DEBUG
        end
    end
end

return love.nogame
