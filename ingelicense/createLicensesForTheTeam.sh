#!/bin/sh
members=("stephaneVales" "alexandreLemort" "vincentPeyrqueou" "heloiseBacle" "justineLimoges" "claireDhoosche" "natanaelVaugien" "mathieuSoum" "chloeRoumieu" "aurelienBroutin" "vincentFougeras" "sandyGiger" "magalieLaguna" "patxiBerard" "pcWindows1" "surfaceProCockpit" "albePCWindows")

mkdir licenses

for m in "${members[@]}"
do
    echo "License for $m"
    ingelicense --expiration 2021/12/31 \
        --editorExpiration 2021/12/31 \
        --customer $m \
        --editorOwner $m \
        --features ./configs/features.txt \
        --agents ./configs/agents.txt \
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
    --editorOwner "Jérome Barbé" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
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
    --editorOwner "Christophe Chastanet" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/Diota.igslicense
echo ""
echo "Read license for Diota"
ingelicense --read ./licenses/Diota.igslicense
echo ""

#licenses for diota
echo "License for Diota Shiva"
ingelicense \
    --id DiotaShiva \
    --customer DiotaShiva \
    --order Eval \
    --expiration 2020/12/31 \
    --editorExpiration 2020/12/31 \
    --editorOwner "Shiva team" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/DiotaShiva.igslicense
echo ""
echo "Read license for Diota Shiva"
ingelicense --read ./licenses/DiotaShiva.igslicense
echo ""

#licenses for eurocontrol
echo "License for Eurocontrol"
ingelicense \
    --id EurocontrolPJ09 \
    --customer "Eurocontrol PJ09" \
    --order Eval \
    --expiration 2050/12/31 \
    --editorExpiration 2050/12/31 \
    --editorOwner "Eurocontrol PJ09" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/EurocontrolPJ09.igslicense
echo ""
echo "Read license for EurocontrolPJ09"
ingelicense --read ./licenses/EurocontrolPJ09.igslicense
echo ""

#licenses for NATO SACT
echo "License for NATO SACT"
ingelicense \
    --id NATO-ACT \
    --customer "NATO ACT" \
    --order I2-20-014-NATO \
    --expiration 2040/12/31 \
    --editorExpiration 2021/07/31 \
    --editorOwner "NATO ACT" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/NATO_SACT.igslicense
echo ""
echo "Read license for NATO ACT"
ingelicense --read ./licenses/NATO_SACT.igslicense
echo ""

#licenses for Waxym
echo "License for Waxym"
ingelicense \
    --id Waxym \
    --customer "Waxym" \
    --order Subcontracting \
    --expiration 2021/03/31 \
    --editorExpiration 2021/03/31 \
    --editorOwner "Waxym" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/Waxym.igslicense
echo ""
echo "Read license for Waxym"
ingelicense --read ./licenses/Waxym.igslicense
echo ""
echo ""

#licenses for Navocap
echo "License for Navocap"
ingelicense \
    --id Navocap \
    --customer "Navocap" \
    --order PCVA \
    --expiration 2021/03/31 \
    --editorExpiration 2021/03/31 \
    --editorOwner "Navocap" \
    --features ./configs/features.txt \
    --agents ./configs/agents.txt \
    --output ./licenses/Navocap.igslicense
echo ""
echo "Read license for Navocap"
ingelicense --read ./licenses/Navocap.igslicense
echo ""

#licenses for Bouyer
echo "License for Bouyer"
ingelicense \
    --id Bouyer \
    --customer "Bouyer" \
    --order Bouyer \
    --expiration 2050/12/31 \
    --editorExpiration 2020/03/31 \
    --editorOwner "Bouyer" \
    --features ./configs/features_bouyer.txt \
    --agents ./configs/agents_bouyer.txt \
    --output ./licenses/Bouyer.igslicense
echo ""
echo "Read license for Bouyer"
ingelicense --read ./licenses/Bouyer.igslicense
echo ""
