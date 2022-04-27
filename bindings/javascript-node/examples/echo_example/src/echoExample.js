//
//  echoExample.js
//
//  Created by Chloé Roumieu on 2022/04/04.
//  Copyright © 2021 Ingenuity i/o. All rights reserved.
//

const igs = require('ingescape');

class EchoExample {

  constructor() {
    //inputs
    this.boolI = false;
    this.integerI = 0;
    this.doubleI = 0;
    this.stringI = "";
    this.dataI = null;

    //outputs
    this.boolO = false;
    this.integerO = 0;
    this.doubleO = 0;
    this.stringO = "";
    this.dataO = null;
  }

  /////////////////////////////////////////////////////////////////////
  //inputs

  // impulsion
  setImpulsionI() {
    this.setImpulsionO();
  }

  // bool
  setBoolI(boolI) {
    this.boolI = boolI;
    this.setBoolO(this.boolI);
  }
  getBoolI() {
    return this.boolI;
  }

  // integer
  setIntegerI(integerI) {
    this.integerI = integerI;
    this.setIntegerO(this.integerI);
  }
  getIntegerI() {
    return this.integerI;
  }

  // double
  setDoubleI(doubleI) {
    this.doubleI = doubleI;
    this.setDoubleO(this.doubleI);
  }
  getDoubleI() {
    return this.doubleI;
  }

  // string
  setStringI(stringI) {
    this.stringI = stringI;
    this.setStringO(this.stringI);
  }
  getStringI() {
    return this.stringI;
  }

  // data
  setDataI(dataI) {
    this.dataI = dataI;
    this.setDataO(this.dataI);
  }
  getDataI() {
    return this.dataI;
  }

  /////////////////////////////////////////////////////////////////////
  //outputs

  // impulsion
  setImpulsionO() {
    igs.outputSetImpulsion("impulsion");
  }

  // bool
  setBoolO(boolO) {
    this.boolO = boolO;
    igs.outputSetBool("bool", this.boolO);
  }
  getBoolO() {
    return this.boolO;
  }

  // integer
  setIntegerO(integerO) {
    this.integerO = integerO;
    igs.outputSetInt("integer", this.integerO);
  }
  getIntegerO() {
    return this.integerO;
  }

  // double
  setDoubleO(doubleO) {
    this.doubleO = doubleO;
    igs.outputSetDouble("double", this.doubleO);
  }
  getDoubleO() {
    return this.doubleO;
  }

  // string
  setStringO(stringO) {
    this.stringO = stringO;
    igs.outputSetString("string", this.stringO);
  }
  getStringO() {
    return this.stringO;
  }

  // data
  setDataO(dataO) {
    this.dataO = dataO;
    igs.outputSetData("data", this.dataO);
  }
  getDataO() {
    return this.dataO;
  }

  /////////////////////////////////////////////////////////////////////
  //services

  // receive_values
  receiveValues(bool, integer, double, string, data) {
    igs.info("Receives values : ");

    igs.info("bool : " + bool);
    igs.info("integer : " + integer);
    igs.info("double : " + double);
    igs.info("string : " + string);
    igs.info("data with byte length : " + (data == null ? 0 : data.byteLength));
  }

  // send_values
  sendValues() {
    let argsList = [];
    argsList = igs.serviceArgsAddBool(argsList, this.boolI);
    argsList = igs.serviceArgsAddInt(argsList, this.integerI);
    argsList = igs.serviceArgsAddDouble(argsList, this.doubleI);
    argsList = igs.serviceArgsAddString(argsList, this.stringI);
    argsList = igs.serviceArgsAddData(argsList, this.dataI);
    igs.serviceCall(toAgent, "receive_values", argsList, "");
  }
};

module.exports = {EchoExample:EchoExample};

