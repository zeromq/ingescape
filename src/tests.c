//
//  mtic_tests.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include "tests.h"
#include "mastic_private.h"

void callback_test_input_1(agent_iop* input_iop){
    fprintf(stderr, "%s - the input name is : %s\n", __FUNCTION__, input_iop->name);
}


void callback_test_input_2(agent_iop* input_iop){
    fprintf(stderr, "%s - the input name is : %s\n", __FUNCTION__, input_iop->name);
}

//void test_copy_definition(){
//    printf(" ------------ test_model_workflow ------------- \n");

//    mtic_definition_loaded = load_definition_from_path("test_definition_my_agent.json");    //Definition of the agent
//    mtic_definition_live = malloc(sizeof(mtic_definition_loaded));
//    copy_definition(mtic_definition_live, mtic_definition_loaded);

//    printf("\t The definition loaded : \n");
//    print_definition(mtic_definition_loaded);


//    printf("The definition live : \n");
//    print_definition(mtic_definition_live);

//    //Free the memory
//    free_definition(mtic_definition_loaded);
//    free_definition(mtic_definition_live);
//}

void test_map(){
    printf(" ------------ test_map ------------- \n");

    //Load definition
    mtic_definition_loaded = load_definition_from_path("test_definition_my_agent.json");
    mtic_definition_live = load_definition_from_path("test_definition_my_agent.json");

    //Load a map json file
    load_map_from_path("test_map.json");

    //test map adding
    mtic_map("e1", "*.s8");

    //Print the mapping corresponding to the output
    print_mapping(mtic_my_agent_mapping);

    //Free the memory
    free_definition(mtic_definition_loaded);
    free_definition(mtic_definition_live);

}

void test_model_workflow(){
    printf(" ------------ test_model_workflow ------------- \n");

    mtic_definition_loaded = load_definition_from_path("test_definition_my_agent.json");    //Definition of the agent
    mtic_definition_live = load_definition_from_path("test_definition_my_agent.json");    //Definition of the agent

    /*
     * Input 1 : e1
     *
     */
    /***** OBSERVE & SET *****/
    double new_value_e1 = 10.12;
    mtic_observe("e1", &callback_test_input_1);
    mtic_set("e1",&new_value_e1);

    /***** GET *****/
    double value_e1 = 0.0;
    model_state *state1 = NULL;
    *state1 = OK;
    value_e1 = *(double*)mtic_get("e1", state1);

    /*
     * Input 2 : e2
     *
     */
    /***** OBSERVE & SET *****/
    char* new_value_e2 = strdup("modification");
    mtic_observe("e2", &callback_test_input_2);
    mtic_set("e2",new_value_e2);

    /***** GET *****/
    char *value_e2 = NULL;
    model_state *state2 = NULL;
    *state2 = OK;
    value_e2 = mtic_get("e2", state2);

    /*
     * Print loaded & live definition
     *
     */
    printf("\t The definition loaded : \n");
    print_definition(mtic_definition_loaded);


    printf("The definition live : \n");
    print_definition(mtic_definition_live);

    //Free the memory
    free_definition(mtic_definition_loaded);
    free_definition(mtic_definition_live);
    free(new_value_e2);
    free(value_e2);
}

void test_mapping_workflow(){
    printf(" ------------ test_mapping_workflow ------------- \n");

    //Load definition
    mtic_definition_loaded = load_definition_from_path("test_definition_my_agent.json");
    mtic_definition_live = load_definition_from_path("test_definition_my_agent.json");

    //Load a map json file
    load_map_from_path("test_map.json");

    /*
     * Simulate the ENTER
     *
     */

    definition *external_agent_def = load_definition_from_path("external_definition.json");

    //Check the map
    agent_iop* output_to_subscribe = mtic_check_map(external_agent_def);
    if(output_to_subscribe != NULL){
    printf("The output(s) following need to be subscribed from agent : %s \n",
           external_agent_def->name);
    print_iop(output_to_subscribe);
    }

    //subscribe : map received
    double value = 25.63;
    model_state state = map_received("A2","s4",&value);

    if(state == NOK)
        printf("The map_received 'A2.s4' is NOK \n");

    //Print the mapping
    print_mapping(mtic_my_agent_mapping);

    agent_iop* output_to_unsubscribe = mtic_unmap(external_agent_def);

    //Print the mapping after unmap
    print_mapping(mtic_my_agent_mapping);

    //Free the memory
    free_definition(mtic_definition_loaded);
    free_definition(mtic_definition_live);
    free_definition(external_agent_def);
    free_mapping(mtic_my_agent_mapping);
    //Free iop map
    struct agent_iop *iop, *tmp;
    HASH_ITER(hh,output_to_unsubscribe, iop, tmp)
    {
        HASH_DEL(output_to_unsubscribe, iop);
        free(iop);
    }
}

void test_check_category(){
    //Load the definition
    mtic_definition_loaded = load_definition_from_path("definition.json");

    //Load the category compatible
    category *cat_compatible = load_category_from_path("cat_compatible.json");

    //Load the category incompatible
    category *cat_incompatible = load_category_from_path("cat_incompatible.json");

    //Test the compatibility
    if(check_category(mtic_definition_loaded,cat_compatible,GLOBAL) == true){
        printf("The agent named %s is compatible with the category named %s \n",
               mtic_definition_loaded->name,
               cat_compatible->name);
    }else{
        printf("The agent named %s is NOT compatible with the category named %s \n",
               mtic_definition_loaded->name,
               cat_compatible->name);
    }

    //Test the incompatibility
    if(check_category(mtic_definition_loaded,cat_incompatible,GLOBAL) == true){
        printf("The agent named %s is compatible with the category named %s \n",
               mtic_definition_loaded->name,
               cat_incompatible->name);
    }else{
        printf("The agent named %s is NOT compatible with the category named %s \n",
               mtic_definition_loaded->name,
               cat_incompatible->name);
    }

}

/*****  print function *****/
void print_mapping(mapping *mapp){


    printf("\n **** mapping **** \n");

    printf("\t name : \t %s \n", mapp->name);
    printf("\t description : \t %s \n", mapp->description);
    printf("\t version : \t %s \n", mapp->version);

    mapping_out *map_out, *m_out;
    mapping_cat *map_cat, *m_cat;

    /* mapping out */
    map_out = mapp->map_out;
    unsigned int hashCount = HASH_COUNT(map_out);
    printf("mapping output[%u]:\n", hashCount);
    for(m_out=map_out; m_out != NULL; m_out=m_out->hh.next) {
        printf("\t [%d] : ( %s, %s.%s ) : %s\n",
        m_out->map_id,
        m_out->input_name,
        m_out->agent_name,
        m_out->output_name,
        map_state_to_string(m_out->state));
    }

    /* mapping cat */
    map_cat = mapp->map_cat;
    hashCount = HASH_COUNT(map_cat);
    printf("mapping category[%u]:\n", hashCount);
    for(m_cat=map_cat; m_cat != NULL; m_cat=m_cat->hh.next) {
        printf("\t [%d] : ( %s , %s ) : %s\n",
        m_cat->map_cat_id,
        m_cat->agent_name,
        m_cat->category_name,
        map_state_to_string(m_cat->state));
    }
}

void print_iop(agent_iop *iop)
{
    printf("%s", iop->name);
    
    switch(iop->type)
    {
        case INTEGER:
            printf(", %s", "INTEGER");
            printf(", %d", iop->value.i);
            break;
            
        case DOUBLE_TYPE:
            printf(", %s", "DOUBLE");
            printf(", %f", iop->value.d);
            break;
            
        case STRING:
            printf(", %s", "STRING");
            printf(", %s", iop->value.s);
            break;
            
        case BOOL_TYPE:
            printf(", %s", "BOOL");
            printf(", %s", iop->value.b ? "true" : "false");
            break;
            
        case IMPULSION:
            printf(", %s", "IMPULSION");
            printf(", %s", iop->value.impuls);
            break;
            
        case STRUCTURE:
            printf(", %s", "STRUCTURE");
            printf(", %s", iop->value.strct);
            break;
            
        default:
            printf(", %s", "undefined");
            break;
    }
    
    printf("\n");
}

void print_category(category *cat)
{
    unsigned int hashCount;
    struct agent_iop *d;
    int i;

    printf("\tcategory name : %s \n", cat->name);
    printf("\tcategory version : %s \n", cat->version);

    /* parameters */
    hashCount = HASH_COUNT(cat->params_table);
    printf("\tparameters[%u]:\n", hashCount);
    i = 0;
    for(d=cat->params_table; d != NULL; d=d->hh.next) {
        printf ("\t\t[%d]: ", ++i);
        print_iop(d);
    }

    /* intputs */
    hashCount = HASH_COUNT(cat->inputs_table);
    printf("\tinputs[%u]:\n", hashCount);
    i = 0;
    for(d=cat->inputs_table; d != NULL; d=d->hh.next) {
        printf ("\t\t[%d]: ", ++i);
        print_iop(d);
    }

    /* outputs */
    hashCount = HASH_COUNT(cat->outputs_table);
    printf("\toutputs[%u]:\n", hashCount);
    i = 0;
    for(d=cat->outputs_table; d != NULL; d=d->hh.next) {
        printf ("\t\t[%d]: ", ++i);
        print_iop(d);
    }
}

void print_definition(definition *def)
{
    unsigned int hashCount;
    struct agent_iop *d;
    int i;
    
    printf("Agent name : %s \n", def->name);
    printf("Agent description : %s \n", def->description);
    printf("Agent version : %s \n", def->version);
    
    /* parameters */
    hashCount = HASH_COUNT(def->params_table);
    printf("\nparameters[%u]:\n", hashCount);
    i = 0;
    for(d=def->params_table; d != NULL; d=d->hh.next) {
        printf ("\t[%d]: ", ++i);
        print_iop(d);
    }

    /* intputs */
    hashCount = HASH_COUNT(def->inputs_table);
    printf("\ninputs[%u]:\n", hashCount);
    i = 0;
    for(d=def->inputs_table; d != NULL; d=d->hh.next) {
        printf ("\t[%d]: ", ++i);
        print_iop(d);
    }

    /* outputs */
    hashCount = HASH_COUNT(def->outputs_table);
    printf("\noutputs[%u]:\n", hashCount);
    i = 0;
    for(d=def->outputs_table; d != NULL; d=d->hh.next) {
        printf ("\t[%d]: ", ++i);
        print_iop(d);
    }

    /* categories */
    struct category *cat;
    hashCount = HASH_COUNT(def->categories);
    printf("\ncategories[%u]:\n", hashCount);
    i = 0;
    for(cat=def->categories; cat != NULL; cat=cat->hh.next) {
        printf ("\t[categeory %d] ---------- \n", ++i);
        print_category(cat);
        printf ("\n");
    }
}

void set_definition(const char *name,
                    const char *description,
                    const char *version,
                    category* categories,
                    agent_iop *params,
                    agent_iop *inputs,
                    agent_iop *outputs)
{
    mtic_definition_live = calloc(1, sizeof(definition));
    
    //Set the agent's name
    mtic_definition_live->name = name;
    
    //Set the agent's definition
    mtic_definition_live->description = description;
    
    //Set the agent's version
    mtic_definition_live->version = version;
    
    //Initialize the agent's categories
    mtic_definition_live->categories = NULL;
    mtic_definition_live->categories = calloc(1, sizeof(category));
    mtic_definition_live->categories = categories;
    
    //Set the agent's parameter
    mtic_definition_live->params_table = NULL;
    mtic_definition_live->params_table = calloc(1, sizeof(agent_iop));
    mtic_definition_live->params_table = params;
    
    //Set the agent's inputs
    mtic_definition_live->inputs_table = NULL;
    mtic_definition_live->inputs_table = calloc(1, sizeof(agent_iop));
    mtic_definition_live->inputs_table = inputs;
    
    //Set the agent's outputs
    mtic_definition_live->outputs_table = NULL;
    mtic_definition_live->outputs_table = calloc(1, sizeof(agent_iop));
    mtic_definition_live->outputs_table = outputs;
}


category *init_categories()
{
    category *categories = NULL;
    
    /**** Define 2 categories ****/
    
    /*
     * Categorie : IHM
     *
     */
    category *ihm_cat = NULL;
    ihm_cat = calloc(1, sizeof(category));
    ihm_cat->name = "ihm";
    ihm_cat->version = "1.0.0";
    
    //Inputs
    ihm_cat->inputs_table  = NULL;
    
    //pointer_e1
    agent_iop *ihm_pointer_e1 = NULL;
    ihm_pointer_e1 = calloc(1, sizeof(agent_iop));
    ihm_pointer_e1->name = "e1";
    ihm_pointer_e1->type = INTEGER;
    ihm_pointer_e1->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->inputs_table , name,  ihm_pointer_e1);
    
    //pointer_e1
    agent_iop *ihm_pointer_e2 = NULL;
    ihm_pointer_e2 = calloc(1, sizeof(agent_iop));
    ihm_pointer_e2->name = "e2";
    ihm_pointer_e2->type = INTEGER;
    ihm_pointer_e2->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->inputs_table , name,  ihm_pointer_e2);
    
    //Outputs
    ihm_cat->outputs_table = NULL;
    
    //pointer_s1
    agent_iop *ihm_pointer_s1 = NULL;
    ihm_pointer_s1 = calloc(1, sizeof(agent_iop));
    ihm_pointer_s1->name = "s1";
    ihm_pointer_s1->type = INTEGER;
    ihm_pointer_s1->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->outputs_table, name,  ihm_pointer_s1);
    
    //pointer_s2
    agent_iop *ihm_pointer_s2 = NULL;
    ihm_pointer_s2 = calloc(1, sizeof(agent_iop));
    ihm_pointer_s2->name = "s2";
    ihm_pointer_s2->type = INTEGER;
    ihm_pointer_s2->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->outputs_table, name,  ihm_pointer_s2);
    
    //Parameters
    ihm_cat->params_table = NULL;
    
    //pointer_p1
    agent_iop *ihm_pointer_p1 = NULL;
    ihm_pointer_p1 = calloc(1, sizeof(agent_iop));
    ihm_pointer_p1->name = "p1";
    ihm_pointer_p1->type = INTEGER;
    ihm_pointer_p1->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->params_table, name,  ihm_pointer_p1);
    
    //pointer_p2
    agent_iop *ihm_pointer_p2 = NULL;
    ihm_pointer_p2 = calloc(1, sizeof(agent_iop));
    ihm_pointer_p2->name = "p2";
    ihm_pointer_p2->type = INTEGER;
    ihm_pointer_p2->value.i = 0;
    
    HASH_ADD_STR( ihm_cat->params_table, name,  ihm_pointer_p2);
    
    /*
     * Categorie : POINTER
     *
     */
    category *pointer_cat = NULL;
    pointer_cat = calloc(1, sizeof(category));
    pointer_cat->name = "pointer";
    pointer_cat->version = "1.0.0";
    
    //Inputs
    pointer_cat->inputs_table = NULL;
    
    //pointer_e1
    agent_iop *pointer_e1 = NULL;
    pointer_e1 = calloc(1, sizeof(agent_iop));
    pointer_e1->name = "e1";
    pointer_e1->type = INTEGER;
    pointer_e1->value.i = 0;
    
    HASH_ADD_STR( pointer_cat->inputs_table , name,  pointer_e1);
    
    //pointer_e2
    agent_iop *pointer_e2 = NULL;
    pointer_e2 = calloc(1, sizeof(agent_iop));
    pointer_e2->name = "e2";
    pointer_e2->type = INTEGER;
    pointer_e2->value.i = 0;
    
    HASH_ADD_STR( pointer_cat->inputs_table , name,  pointer_e2);
    
    //Outputs
    pointer_cat->outputs_table = NULL;
    
    //pointer_s1
    agent_iop *pointer_s1 = NULL;
    pointer_s1 = calloc(1, sizeof(agent_iop));
    pointer_s1->name = "s1";
    pointer_s1->type = INTEGER;
    pointer_s1->value.i = 0;
    
    HASH_ADD_STR( pointer_cat->outputs_table, name,  pointer_s1);
    
    //pointer_s2
    agent_iop *pointer_s2 = NULL;
    pointer_s2 = calloc(1, sizeof(agent_iop));
    pointer_s2->name = "s2";
    pointer_s2->type = INTEGER;
    pointer_s2->value.i = 0;
    
    HASH_ADD_STR( pointer_cat->outputs_table, name,  pointer_s2);
    
    //Parameters
    pointer_cat->params_table = NULL;
    
    //pointer_p1
    agent_iop *pointer_p1 = NULL;
    pointer_p1 = calloc(1, sizeof(agent_iop));
    pointer_p1->name = "p1";
    pointer_p1->type = INTEGER;
    pointer_p1->value.i = 0;
    
    HASH_ADD_STR(  pointer_cat->params_table , name,  pointer_p1);
    
    //pointer_p2
    agent_iop *pointer_p2 = NULL;
    pointer_p2 = calloc(1, sizeof(agent_iop));
    pointer_p2->name = "p2";
    pointer_p2->type = INTEGER;
    pointer_p2->value.i = 0;
    
    HASH_ADD_STR(  pointer_cat->params_table , name,  pointer_p2);
    
    //Add to the final table
    HASH_ADD_STR(categories, name, ihm_cat);
    HASH_ADD_STR(categories, name, pointer_cat);
    
    return categories;
}

void init_def()
{
    /*
     * Define the name, description, version of the agent
     *
     */
    const char* name = "mouse";
    const char* description = "Agent mouse : send x, y";
    const char* version = "1.0.0";
    
    /*
     * Initialize 2 outputs
     *
     */
    agent_iop* outputs = NULL;
    
    //int x
    agent_iop *x = NULL;
    x = calloc(1, sizeof(agent_iop));
    x->name = "x";
    x->type = INTEGER;
    x->value.i = 0;
    
    //Add the output 'x' to the table 'outputs'
    HASH_ADD_STR(outputs, name, x);
    
    //int y
    agent_iop *y = NULL;
    y = calloc(1, sizeof(agent_iop));
    y->name = "y";
    y->type = INTEGER;
    y->value.i = 0;
    
    //Add the output 'y' to the table 'outputs'
    HASH_ADD_STR(outputs, name, y);
    
    /*
     * Set the definition of the agent
     *
     */
    set_definition(name,
                   description,
                   version,
                   NULL,
                   NULL,
                   NULL,
                   outputs);
    
    /*
     * TODO: parse the files JSON in the categories folder, for the moment init testing categories
     * Initialize the test & temp categories
     * Check the categories which the agent could be in accordance with
     *
     */
    category* compatible_categories = NULL;
    
    category* all_categories = NULL;
    all_categories = init_categories();
    
    //Test all the categories
    struct category *cat_to_check;
    for(cat_to_check = all_categories; cat_to_check != NULL; cat_to_check = cat_to_check->hh.next) {
        if(check_category(mtic_definition_live, cat_to_check, OUTPUT) == true)
        {
            printf("the agent is compatible to the category : %s \n", cat_to_check->name);
            HASH_ADD_STR(compatible_categories, name, cat_to_check);
        }else
        {
            printf("the agent is not compatible to the category named : %s \n", cat_to_check->name);
        }
    }
    
    
    
    /*
     * Add the compatible categories to the definition
     *
     */
    
}

void parse(char buffer[],int* num_of_args, char **arguments)
{
    char *p = NULL;
    char c;
    *num_of_args = 0;
    
    /* On supprime le de la fin */
    if(NULL != (p = strrchr(buffer, '\n')))
        *p = '\0';
    else
        while(' ' != (c = fgetc(stdin)) && c != EOF);
    
    /* On alloue argv */
    p = strtok(buffer, " ");
    while(p != NULL)
    {
        /*
         * Ici p est un pointeur sur une chaine
         * qui contient exactement l'argument i
         */
        if(*num_of_args < 10)
        {
            arguments[*num_of_args] = malloc(sizeof(char) * (1+strlen(p)));
            strcpy(arguments[*num_of_args], p);
            (*num_of_args)++;
        }
        else
            break;
        /*
         * On lance un nouvel appel a strtok
         * par contre on lui donne en argument NULL
         * pour qu'il sache que c'est celle du dernier
         * appel, on peut aussi changer les caractères
         * pour parser...
         */
        p = strtok(NULL, " ");
    }
    
    arguments[*num_of_args] = NULL;
}
