#!/bin/sh
members=("stephaneVales" "alexandreLemort" "vincentPeyrqueou" "heloiseBacle" "justineLimoges" "emmaChartier" "fabienEsteveny" "claireDhoosche" "natanaelVaugien" "mathieuSoum" "emilieEscalie" "laetitiaBornes" "chloeRoumieu" "patxiBerard" "pcWindows1")

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
