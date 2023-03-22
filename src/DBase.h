/**
 * File:   DBase.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#ifndef DBASE_H
#define DBASE_H

#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sys/stat.h>

#include "Utils.h"

typedef enum
{
  DB_OK,
  DB_ERR

} DBStatusCode;

typedef enum
{
  TKT_ONG, // Ongoing
  TKT_DON, // Done
  TKT_HID  // Hidden

} TicketState;

const char *TicketStateToStr(TicketState state);
std::string get_file_extension(const std::string &path);
std::string get_file_name(const std::string &path);

typedef struct
{
  size_t id;
  std::string name;

} Group;

typedef struct
{
  size_t id;
  std::string name;
  std::string path;

} Attachment;

typedef struct
{
  size_t id;
  uint8_t progress;
  std::string desc;
  std::string resource;
  uint64_t eta;

} TicketTask;

typedef struct
{
  size_t id;
  size_t group_id;
  TicketState state;
  uint64_t creation_date;
  std::string name;
  std::string desc;
  std::vector<Attachment> attachments;
  std::vector<TicketTask> tasks;

} Ticket;

class DBase
{
public:
  DBase(const std::string &db_name);
  ~DBase();

  DBStatusCode Status();
  std::string &Path();

  void LoadData();
  void SaveData();

  void AddGroup(std::string group_name);
  void EditGroup(size_t id, std::string new_name);
  void DeleteGroup(size_t id);
  Group GetGroup(size_t id);

  void AddTicket
  (
    size_t group_id,
    std::string ticket_name,
    std::string ticket_desc
  );
  void EditTicket
  (
    size_t id,
    std::string ticket_name,
    std::string ticket_desc
  );
  void DeleteTicket(size_t id);
  Ticket GetTicket(size_t id);
  void MoveTicketToGroup(size_t id, size_t new_group_id);
  void ChangeTicketState(size_t id, TicketState new_state);
  void AddTicketAttachment(size_t id, std::string orig_path);
  void OpenTicketAttachment(size_t id, size_t attachment_id);
  void NewTextTicketAttachment(size_t id, std::string filename);
  void EditTicketAttachment
  (
    size_t id,
    size_t attachment_id,
    std::string new_attachment_path
  );
  void DeleteTicketAttachment(size_t id, size_t attachment_id);
  Attachment GetTicketAttachment(size_t id, size_t attachment_id);
  void AddTicketTask
  (
    size_t id,
    uint64_t eta,
    std::string resource,
    std::string desc
  );
  void EditTicketTaskETA
  (
    size_t id,
    size_t task_id,
    uint64_t new_eta
  );
  void EditTicketTaskResource
  (
    size_t id,
    size_t task_id,
    std::string new_resource
  );
  void EditTicketTaskDesc
  (
    size_t id,
    size_t task_id,
    std::string new_desc
  );
  void IncTicketTaskProgress(size_t id, size_t task_id);
  void DecTicketTaskProgress(size_t id, size_t task_id);
  void DeleteTicketTask(size_t id, size_t task_id);
  TicketTask GetTicketTask(size_t id, size_t task_id);

  std::vector<Group> GetGroups();
  std::vector<Ticket> GetTickets(size_t group_id);
  std::vector<Ticket> GetDoneTickets();
  std::vector<Ticket> GetOpenTickets();
  std::vector<Ticket> GetHiddenTickets();
  std::vector<Ticket> GetAllTickets();

private:
  void _InitDB();
  void _LoadGroups();
  void _SaveGroups();
  size_t _GetGroupsLastId();
  bool _GroupHasTickets(uint64_t id);
  
  void _LoadTickets();
  void _SaveTickets();
  size_t _GetTicketsLastId();
  size_t _GetTicketsAttachmentsLastId(size_t ticket_id);
  size_t _GetTicketsTasksLastId(size_t ticket_id);

  DBStatusCode status_code;
  std::string db_path;
  std::string dv_path;
  std::string db_name;

  std::vector<Group> groups;
  std::vector<Ticket> tickets;
};

#endif /* ifndef DBASE_H */
