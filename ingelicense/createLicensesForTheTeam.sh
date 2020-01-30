#!/bin/sh
members=("stephaneVales" "alexandreLemort" "vincentPeyrqueou" "heloiseBacle" "justineLimoges" "emmaChartier" "fabienEsteveny" "claireDhoosche" "natanaelVaugien" "mathieuSoum" "emilieEscalie" "laetitiaBornes" "chloeRoumieu" "aurelienBroutin" "patxiBerard" "pcWindows1" "surfaceProCockpit" "albePCWindows")

mkdir licenses

for m in "${members[@]}"
do
    echo "License for $m"
    ingelicense --expiration 2021/12/31 \
        --editorExpiration 2021/12/31 \
        --features /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/features.txt \
        --agents /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/agents.txt \
        --customer $m \
        --editorOwner $m \
        --features ./src/features.txt \
        --agents ./src/agents.txt \
        --output ./licenses/$m.igslicense
    echo ""
done

#read verification
for m in "${members[@]}"
do
    echo "Read license for $m"
    ingelicense --read ./licenses/$m.igslicense
    echo ""
done

#licenses for airbus
echo "License for AirbusControlDeck"
ingelicense \
    --id AirbusControlDeck \
    --customer Airbus \
    --order ORFE \
    --expiration 2050/12/31 \
    --editorExpiration 2050/12/31 \
    --features /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/features.txt \
    --agents /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/agents.txt \
    --editorOwner "Jérome Barbé" \
    --features ./src/features.txt \
    --agents ./src/agents.txt \
    --output ./licenses/AirbusControlDeck.igslicense
echo ""
echo "Read license for AirbusControlDeck"
ingelicense --read ./licenses/AirbusControlDeck.igslicense
echo ""

#licenses for diota
echo "License for Diota"
ingelicense \
    --id Diota \
    --customer Diota \
    --order Eval \
    --expiration 2020/12/31 \
    --editorExpiration 2020/12/31 \
    --features /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/features.txt \
    --agents /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/agents.txt \
    --editorOwner "Christophe Chastanet" \
    --features ./src/features.txt \
    --agents ./src/agents.txt \
    --output ./licenses/Diota.igslicense
echo ""
echo "Read license for Diota"
ingelicense --read ./licenses/Diota.igslicense
echo ""

#licenses for diota
echo "License for Diota Shiva"
ingelicense \
    --id Diota \
    --customer DiotaShiva \
    --order Eval \
    --expiration 2020/12/31 \
    --editorExpiration 2020/12/31 \
    --features /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/features.txt \
    --agents /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/agents.txt \
    --editorOwner "Shiva team" \
    --features ./src/features.txt \
    --agents ./src/agents.txt \
    --output ./licenses/DiotaShiva.igslicense
echo ""
echo "Read license for Diota Shiva"
ingelicense --read ./licenses/DiotaShiva.igslicense
echo ""

#licenses for eurocontrol
echo "License for Eurocontrol"
ingelicense \
    --id Diota \
    --customer "Eurocontrol PJ09" \
    --order Eval \
    --expiration 2020/12/31 \
    --editorExpiration 2020/12/31 \
    --features /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/features.txt \
    --agents /Users/steph/Documents/CloudStation/ingescape/code/ingescape/ingelicense/src/agents.txt \
    --editorOwner "Eurocontrol PJ09" \
    --features ./src/features.txt \
    --agents ./src/agents.txt \
    --output ./licenses/EurocontrolPJ09.igslicense
echo ""
echo "Read license for EurocontrolPJ09"
ingelicense --read ./licenses/EurocontrolPJ09.igslicense
echo ""
