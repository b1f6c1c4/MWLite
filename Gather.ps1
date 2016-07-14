Function Gather-MWFile($fn)
{
    $fnx = Split-Path $fn -Parent
    $fnc = Split-Path $fnx -Leaf
    $fnd = Split-Path $fnx -Parent

    $fng = "$fnd\$fnc.log"

    if (!(Test-Path $fng))
    {
        Copy-Item $fn $fng
        Move-Item $fn "$fn.bak"
        return
    }

    $db = @(Get-Content $fng)
    $cont = @(Get-Content $fn)

    $i = 0
    while ($i -lt $cont.Count)
    {
        $db[$i] += $cont[$i]
        i++
    }
}
