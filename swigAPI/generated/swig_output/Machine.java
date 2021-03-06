/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.sysfera.vishnu.api.vishnu.internal;

public class Machine extends EObject {
  private long swigCPtr;

  protected Machine(long cPtr, boolean cMemoryOwn) {
    super(VISHNUJNI.SWIGMachineUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Machine obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        VISHNUJNI.delete_Machine(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public Machine() {
    this(VISHNUJNI.new_Machine(), true);
  }

  public void _initialize() {
    VISHNUJNI.Machine__initialize(swigCPtr, this);
  }

  public String getMachineId() {
    return VISHNUJNI.Machine_getMachineId(swigCPtr, this);
  }

  public void setMachineId(String _machineId) {
    VISHNUJNI.Machine_setMachineId(swigCPtr, this, _machineId);
  }

  public String getName() {
    return VISHNUJNI.Machine_getName(swigCPtr, this);
  }

  public void setName(String _name) {
    VISHNUJNI.Machine_setName(swigCPtr, this, _name);
  }

  public String getSite() {
    return VISHNUJNI.Machine_getSite(swigCPtr, this);
  }

  public void setSite(String _site) {
    VISHNUJNI.Machine_setSite(swigCPtr, this, _site);
  }

  public String getMachineDescription() {
    return VISHNUJNI.Machine_getMachineDescription(swigCPtr, this);
  }

  public void setMachineDescription(String _machineDescription) {
    VISHNUJNI.Machine_setMachineDescription(swigCPtr, this, _machineDescription);
  }

  public String getLanguage() {
    return VISHNUJNI.Machine_getLanguage(swigCPtr, this);
  }

  public void setLanguage(String _language) {
    VISHNUJNI.Machine_setLanguage(swigCPtr, this, _language);
  }

  public int getStatus() {
    return VISHNUJNI.Machine_getStatus(swigCPtr, this);
  }

  public void setStatus(int _status) {
    VISHNUJNI.Machine_setStatus(swigCPtr, this, _status);
  }

  public String getSshPublicKey() {
    return VISHNUJNI.Machine_getSshPublicKey(swigCPtr, this);
  }

  public void setSshPublicKey(String _sshPublicKey) {
    VISHNUJNI.Machine_setSshPublicKey(swigCPtr, this, _sshPublicKey);
  }

  public SWIGTYPE_p_ecorecpp__mapping__any eGet(int _featureID, boolean _resolve) {
    return new SWIGTYPE_p_ecorecpp__mapping__any(VISHNUJNI.Machine_eGet(swigCPtr, this, _featureID, _resolve), true);
  }

  public void eSet(int _featureID, SWIGTYPE_p_ecorecpp__mapping__any _newValue) {
    VISHNUJNI.Machine_eSet(swigCPtr, this, _featureID, SWIGTYPE_p_ecorecpp__mapping__any.getCPtr(_newValue));
  }

  public boolean eIsSet(int _featureID) {
    return VISHNUJNI.Machine_eIsSet(swigCPtr, this, _featureID);
  }

  public void eUnset(int _featureID) {
    VISHNUJNI.Machine_eUnset(swigCPtr, this, _featureID);
  }

  public SWIGTYPE_p_ecore__EClass _eClass() {
    long cPtr = VISHNUJNI.Machine__eClass(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_ecore__EClass(cPtr, false);
  }

}
