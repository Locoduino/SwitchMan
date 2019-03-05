
/*
 * Creation des objets qui composent le reseau
 */
/*
 * Les voies
 */
TRACK(BlockTrack, voie0);
TRACK(BlockTrack, voie1);
TRACK(BlockTrack, voie2);
TRACK(BlockTrack, voie3);
TRACK(BlockTrack, voie4);
TRACK(BlockTrack, voie5);
TRACK(BlockTrack, voie6);
TRACK(BlockTrack, voie7);
TRACK(BlockTrack, voie8);
TRACK(BlockTrack, voie9);
TRACK(BlockTrack, voie14);
TRACK(BlockTrack, voie16);
TRACK(BlockTrack, voie17);
TRACK(BlockTrack, voie18);
TRACK(BlockTrack, voie19);
TRACK(BlockTrack, voie20);
TRACK(BlockTrack, voie21);
TRACK(BlockTrack, voie22);
TRACK(BlockTrack, voie23);
TRACK(BlockTrack, voie24);
TRACK(BlockTrack, voie25);
TRACK(BlockTrack, voie27);
TRACK(BlockTrack, voie28);
TRACK(BlockTrack, voie29);
/*
 * Les voies de garage
 */
TRACK(DeadendTrack, voieGarage10);
TRACK(DeadendTrack, voieGarage11);
TRACK(DeadendTrack, voieGarage12);
TRACK(DeadendTrack, voieGarage13);
TRACK(DeadendTrack, voieGarage15);
TRACK(DeadendTrack, voieGarage26);
TRACK(DeadendTrack, voieGarage30);
/*
 * Les aiguilles
 */
TRACK(TurnoutTrack, aiguille0);
TRACK(TurnoutTrack, aiguille1);
TRACK(TurnoutTrack, aiguille2);
TRACK(TurnoutTrack, aiguille3);
TRACK(TurnoutTrack, aiguille4);
TRACK(TurnoutTrack, aiguille5);
TRACK(TurnoutTrack, aiguille6);
TRACK(TurnoutTrack, aiguille7);
TRACK(TurnoutTrack, aiguille8);
TRACK(TurnoutTrack, aiguille9);
TRACK(TurnoutTrack, aiguille10);
TRACK(TurnoutTrack, aiguille11);
TRACK(TurnoutTrack, aiguille12);
TRACK(TurnoutTrack, aiguille13);
TRACK(TurnoutTrack, aiguille14);
TRACK(TurnoutTrack, aiguille15);
TRACK(TurnoutTrack, aiguille16);
TRACK(TurnoutTrack, aiguille17);
TRACK(TurnoutTrack, aiguille18);
TRACK(TurnoutTrack, aiguille19);
TRACK(TurnoutTrack, aiguille20);

void export_setup() {
    /*
     * Connexions des voies
     */
    // metro
    voieGarage30.connect(OUTLET, voie29, INLET);
    voie29.connect(OUTLET, voie28, INLET);
    voie28.connect(OUTLET, aiguille15, RIGHT_OUTLET);
    aiguille15.connect(INLET, voie27, INLET);
    voie27.connect(OUTLET, voieGarage26, OUTLET);
    // voie circulaire exterieure
    voie4.connect(OUTLET, aiguille16, INLET);
    aiguille16.connect(LEFT_OUTLET, aiguille15, LEFT_OUTLET);
    aiguille16.connect(RIGHT_OUTLET, aiguille13, RIGHT_OUTLET);
    voie3.connect(OUTLET, aiguille13, LEFT_OUTLET);
    aiguille13.connect(INLET, voie5, INLET);
    voie5.connect(OUTLET, voie6, INLET);
    voie6.connect(OUTLET, voie7, INLET);
    voie7.connect(OUTLET, voie8, INLET);
    voie8.connect(OUTLET, voie9, INLET);
    voie9.connect(OUTLET, aiguille8, RIGHT_OUTLET);
    aiguille8.connect(INLET, aiguille7, INLET);
    aiguille7.connect(RIGHT_OUTLET, voie0, INLET);
    aiguille7.connect(LEFT_OUTLET, aiguille6, LEFT_OUTLET);
    aiguille6.connect(INLET, voie1, INLET);
    voie0.connect(OUTLET, aiguille4, LEFT_OUTLET);
    voie1.connect(OUTLET, aiguille5, INLET);
    aiguille5.connect(RIGHT_OUTLET, aiguille4, RIGHT_OUTLET);
    aiguille4.connect(INLET, aiguille2, INLET);
    aiguille2.connect(LEFT_OUTLET, voie2, INLET);
    voie2.connect(OUTLET, aiguille17, INLET);
    aiguille17.connect(LEFT_OUTLET, voie4, INLET);
    aiguille17.connect(RIGHT_OUTLET, voie3, INLET);
    // voie circulaire interieure en sens inverse de la circulation
    aiguille2.connect(RIGHT_OUTLET, aiguille0, RIGHT_OUTLET);
    aiguille0.connect(INLET, voie25, INLET);
    voie25.connect(OUTLET, aiguille18, INLET);
    aiguille18.connect(LEFT_OUTLET, voie23, INLET);
    aiguille18.connect(RIGHT_OUTLET, voie24, INLET);
    voie23.connect(OUTLET, aiguille14, RIGHT_OUTLET);
    voie24.connect(OUTLET, aiguille14, LEFT_OUTLET);
    aiguille14.connect(INLET, voie22, INLET);
    voie22.connect(OUTLET, voie21, INLET);
    voie21.connect(OUTLET, voie20, INLET);
    voie20.connect(OUTLET, voie19, INLET);
    voie19.connect(OUTLET, voie18, INLET);
    voie18.connect(OUTLET, aiguille11, INLET);
    aiguille11.connect(LEFT_OUTLET, aiguille8, LEFT_OUTLET);
    aiguille11.connect(RIGHT_OUTLET, aiguille9, INLET);
    aiguille9.connect(LEFT_OUTLET, voie16, INLET);
    aiguille9.connect(RIGHT_OUTLET, voie17, INLET);
    voie17.connect(OUTLET, aiguille3, INLET);
    aiguille3.connect(RIGHT_OUTLET, voieGarage15, OUTLET);
    aiguille3.connect(LEFT_OUTLET, aiguille1, LEFT_OUTLET);
    voie16.connect(OUTLET, aiguille1, RIGHT_OUTLET);
    aiguille1.connect(INLET, aiguille0, LEFT_OUTLET);
    // depot
    voieGarage11.connect(OUTLET, aiguille12, LEFT_OUTLET);
    voieGarage12.connect(OUTLET, aiguille12, RIGHT_OUTLET);
    voieGarage13.connect(OUTLET, aiguille10, RIGHT_OUTLET);
    aiguille12.connect(INLET, aiguille10, LEFT_OUTLET);
    aiguille10.connect(INLET, aiguille20, INLET);
    aiguille20.connect(RIGHT_OUTLET, aiguille6, RIGHT_OUTLET);
    aiguille20.connect(LEFT_OUTLET, voie14, INLET);
    voie14.connect(OUTLET, aiguille19, RIGHT_OUTLET);
    aiguille5.connect(LEFT_OUTLET, aiguille19, LEFT_OUTLET);
    aiguille19.connect(INLET, voieGarage10, OUTLET);
}
