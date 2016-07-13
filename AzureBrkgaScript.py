'''
To import this libraries we have to install
sudo apt-get install azure
pip install azure-storage==0.32.0
pip install azure-servicemanagement-legacy==0.20.3
'''
import time

from azure import *
from azure.servicemanagement import *
from azure.storage.blob import BlockBlobService
from azure.storage.blob import ContentSettings
from azure.storage.blob.models import PublicAccess
import json
import paramiko
from scp import SCPClient
from shutil import copy2
from os import mkdir, remove, path
import random



#https://azure.microsoft.com/en-us/documentation/articles/cloud-services-python-how-to-use-service-management/

#f = open('configFile', 'w');

#subscription_id = 'da9c7e61-8353-43d7-9e9d-709d834d82fd'
with open('azure_config.json') as cfg_file:    
    cfg = json.load(cfg_file)

subscription_id = cfg["subscription_id"]
certificate_path = cfg["certificate_path"]
number_machines = int(cfg["number_machines"])
brkga_path = cfg["brkga_path"]

sms = ServiceManagementService(subscription_id, certificate_path)

'''
Function that wait until a operation is done
'''
def wait_for_async(request_id, operation_wait_name, operation_name, timeout):
     count = 0
     result = sms.get_operation_status(request_id) 
     while result.status == 'InProgress':
         count = count + 1
         if count > timeout:
             print('Timed out waiting for async operation to complete.')
             return
         time.sleep(10)
         #print('.')
         result = sms.get_operation_status(request_id)
         #print(vars(result))
         print(operation_wait_name)
         if result.error: 
             print(result.error.code)
             print(vars(result.error))
     print(result.status)
     print(operation_name + ' took: ' + str(count*5) + 's') 


'''
Global Variables
'''

location = "Central US"

'''
Creating storage account
'''

hash_number = hash(subscription_id)%100000000
hash_number = hash_number + 1
name_account = "azbraccount" + str(hash_number)
label = name_account
desc = 'Storage Account for the azure-brkga project.'

storage_accounts = sms.list_storage_accounts()

if len(storage_accounts)==0:
    available = sms.check_storage_account_name_availability(name_account)
    if available.result == False:
        print("Name account already exists")
        exit(1)
    storage_account = sms.create_storage_account(name_account, desc, label, location = location)
    wait_for_async(storage_account.request_id, "Creating storage account", "Create storage account", 1000)
    print("The storage " + name_account + " was created.")
    # Just to validate the creation of storage account
    #operation_result = sms.get_operation_status(result.request_id)
    #print('Operation status: ' + operation_result.status)
    #if operation_result.status==
else:
    storage_account = storage_accounts[0]
    name_account = storage_account.service_name
    print("The storage account was taken of a EXISTING storage account.")


'''
Get the keys for create a container where would be the the virtual hard disks
'''
keys = sms.get_storage_account_keys(name_account)
primary_key = keys.storage_service_keys.primary
'''
Create a container
'''
block_blob_service = BlockBlobService(account_name = name_account, account_key = primary_key )
name_container = "azbrcontainer" + str(hash_number)
list_containers = list(block_blob_service.list_containers())
if len(list_containers)==0:
    container = block_blob_service.create_container(name_container)
    #wait_for_async(container.request_id, 'Creating storage account', 600)


list_containers = list(block_blob_service.list_containers())
container = list_containers[0]
name_container = container.name

block_blob_service.set_container_acl(name_container, public_access=PublicAccess.Container)


''' FINISHING CREAION OF ACCOUNTS OR GET EXISTEN ACCOUNDS'''


'''Create of deplyment and in each deployment a machine'''
 
for number_machine in range(0, number_machines):
    name = 'azbrvm' + str(hash_number) + str(number_machine)
    hosted_service = sms.create_hosted_service(service_name=name, label=name, location=location)
    wait_for_async(hosted_service.request_id, "Creating hosted service", "Create hosted service", 1000)
    right_image = '0b11de9248dd4d87b18621318e037d37__RightImage-Ubuntu-14.04-x64-v14.2.1'
    vhd = 'azbrvhd' + str(hash_number) + str(number_machine)
    media_link  = 'https://'+name_account+'.blob.core.windows.net/'+name_container+'/'+ vhd + '.vhd'
    network_cfg = ConfigurationSet()
    network_cfg.configuration_set_type = 'NetworkConfiguration'
    network_cfg.input_endpoints.input_endpoints.append(
                             ConfigurationSetInputEndpoint(name="SSH",
                                                  protocol="tcp",
                                                  port="22",
                                                  local_port="22"))
    #{"name" : "ssh", "protocol" : "tcp", "port" : "22022", "local_port" : "22"},
    # Linux VM configuration, you can use WindowsConfigurationSet
    # for a Windows VM instead
    linux_config = LinuxConfigurationSet('azurebrkgahost', 'azurebrkga', 'Azurebrkga1', False)
    os_hd = OSVirtualHardDisk(right_image, media_link)
    virtual_machine = sms.create_virtual_machine_deployment(service_name=name,
        deployment_name=name,
        deployment_slot='production',
        label=name,
        role_name=name,
        system_config=linux_config,
        os_virtual_hard_disk=os_hd,
        role_size='ExtraSmall',
        network_config=network_cfg
        )
    wait_for_async(virtual_machine.request_id, "Creating virtual machine " + str(number_machine), "Create virtual machine " + str(number_machine), 1000)



'''
connecting with the machines with paramiko
'''

if not path.exists("Outputs"):
    mkdir("Outputs")

for number_machine in range(0, number_machines):
    name = 'azbrvm' + str(hash_number) + str(number_machine)
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    dns = name + '.cloudapp.net'
    for i in range(100):
        try:
            ssh.connect(dns, username='azurebrkga', password='Azurebrkga1')
            break
        except Exception:
            print("The machine " + str(number_machine) + " is not started yet!!")
            time.sleep(5)
    scp = SCPClient(ssh.get_transport())
    scp.put(brkga_path, 'brkga.tar.xz')   
    print(">>> copy of the brkga to the virtual machine " + str(number_machine) + " was executed")
    ssh.exec_command("tar xf brkga.tar.xz")
    print(">>> tar xf brkga.tar.xz")
    stdin, stdout, stderr = ssh.exec_command("sudo apt-get update")
    stdin.write('Azurebrkga1\n')
    stdin.flush()
    tmp = stdout.read()
    print(">>> sudo apt-get update was executed")
    stdin, stdout, stderr = ssh.exec_command("sudo apt-get install -y build-essential")
    stdin.write('Azurebrkga1\n')
    stdin.flush()
    tmp = stdout.read()
    print(">>> sudo apt-get install -y build-essential was executed")
    stdin, stdout, stderr = ssh.exec_command("cd brkga; make clean")
    tmp = stdout.read()
    print(">>> make clean was executed")
    stdin, stdout, stderr = ssh.exec_command("cd brkga; make")
    tmp = stdout.read()
    print(">>> make was executed")


'''
RUNING IN PARALLEL IN THE MACHINES
'''
stdouts = [0 for i in range(number_machines)]

for number_machine in range(0, number_machines):
    name = 'azbrvm' + str(hash_number) + str(number_machine)
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    dns = name + '.cloudapp.net'
    for i in range(100):
        try:
            ssh.connect(dns, username='azurebrkga', password='Azurebrkga1')
            break
        except Exception:
            print("The machine " + str(number_machine) + " is not started yet!!")
            time.sleep(5)
    seed = random.randint(1,99999999)
    stdin, stdouts[number_machine], stderr = ssh.exec_command("cd brkga; ./brkga config " + str(seed))
    #tmp = stdout.read()
    print(">>> the brkga was executed")

'''
WAITING FOR THE OUTPUTS
'''
for number_machine in range(0, number_machines):
    stdouts[number_machine].read()

'''
COMPRESING THE OUTPUTS AND COPYING TO THE SCRIPT
'''
for number_machine in range(0, number_machines):
    name = 'azbrvm' + str(hash_number) + str(number_machine)
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    dns = name + '.cloudapp.net'
    for i in range(100):
        try:
            ssh.connect(dns, username='azurebrkga', password='Azurebrkga1')
            break
        except Exception:
            print("The machine " + str(number_machine) + " is not started yet!!")
            time.sleep(5)
    stdin, stdout, stderr = ssh.exec_command("cd brkga; tar -cvJf outputs" + str(number_machine) + ".tar.xz Output")
    tmp = stdout.read()
    print(">>> compress the directory where is the output")
    scp = SCPClient(ssh.get_transport())
    scp.get("brkga/outputs" + str(number_machine) + ".tar.xz")
    #copy2("outputs" + str(number_machine) + ".tar.xz", "Outputs/")
    #remove("outputs" + str(number_machine) + ".tar.xz")


#to get from the virtual machine to my script scp.get()
#get the outputs for print as output of the script    


print("The script was finished, you can see the outputs by machine in the directory Outputs\n")
print("Let to the script erase the created machines\n")

for number_machine in range(0, number_machines):
    name = 'azbrvm' + str(hash_number) + str(number_machine)
    
    try:
        result = sms.delete_deployment(name ,name,  delete_vhd = True)
        wait_for_async(result.request_id, "Deleting Deployment", "Delete Deployment", 1000)
    except:
        print("No Deployment")
    try:
        result = sms.delete_hosted_service(name, complete = True)
        wait_for_async(result.request_id,"Deleting Cloud Service", "Delete Cloud Service", 1000)
    except:
        print("No Cloud Service")


time.sleep(400)
block_blob_service.delete_container(name_container)
time.sleep(20)
sms.delete_storage_account(name_account)

