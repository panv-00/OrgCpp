/**
 * File:   DBase.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#include "DBase.h"

const char *TicketStateToStr(TicketState state)
{
  switch (state)
  {
    case TKT_ONG: return "Ongoing";
    case TKT_DON: return "Done";
    case TKT_DEL: return "Deleted";
  }

  return "Undefined";
}

DBase::DBase(const std::string &db_name) : db_name{db_name}
{
  _InitDB();
}

DBase::~DBase()
{

}

DBStatusCode DBase::Status()
{
  return status_code;
}

std::string &DBase::Path()
{
  return db_path;
}

void DBase::LoadData()
{
  _LoadGroups();
  _LoadTickets();
}

void DBase::SaveData()
{
  _SaveGroups();
  _SaveTickets();
}

void DBase::AddGroup(std::string group_name)
{
  for (auto &group : groups)
  {
    if (group.name == group_name)
    {
      // group with the same name already exists
      status_code = DB_ERR;
      return;
    }
  }

  size_t last_id = _GetGroupsLastId();
  Group new_group = {last_id + 1, group_name};

  groups.push_back(new_group);

  status_code = DB_OK;
}

void DBase::EditGroup(size_t id, std::string new_name)
{
  for (auto &group : groups)
  {
    if (group.name == new_name)
    {
      // group with the same name already exists
      status_code = DB_ERR;
      return;
    }
  }

  for (auto it = groups.begin(); it != groups.end(); ++it)
  {
    if (it->id == id)
    {
      it->name = new_name;
      status_code = DB_OK;
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::DeleteGroup(size_t id)
{
  if (_GroupHasTickets(id))
  {
    status_code = DB_ERR;
    return;
  }

  for (auto it = groups.begin(); it != groups.end(); ++it)
  {
    if (it->id == id)
    {
      groups.erase(it);
      status_code = DB_OK;
      return;
    }
  }

  status_code = DB_ERR;
}

Group DBase::GetGroup(size_t id)
{
  for (auto it = groups.begin(); it != groups.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      return *it;
    }
  }

  status_code = DB_ERR;
  return Group {0, ""};
}

void DBase::AddTicket
(
  size_t group_id,
  std::string ticket_name,
  std::string ticket_desc
)
{
  for (auto &ticket : tickets)
  {
    if (ticket.name == ticket_name)
    {
      // ticket with the same name already exists
      status_code = DB_ERR;
      return;
    }
  }

  size_t last_id = _GetTicketsLastId();
  std::vector<Attachment> new_attachments;
  std::vector<TicketTask> new_tasks;

  Ticket new_ticket =
  {
    last_id + 1,
    group_id,
    TKT_ONG,
    Now(),
    ticket_name,
    ticket_desc,
    new_attachments,
    new_tasks
  };

  tickets.push_back(new_ticket);
  status_code = DB_OK;
}

void DBase::EditTicket
(
  size_t id,
  std::string ticket_name,
  std::string ticket_desc
)
{
  for (auto &ticket : tickets)
  {
    if (ticket.name == ticket_name && ticket.id != id)
    {
      // another ticket with the same name already exists
      status_code = DB_ERR;
      return;
    }
  }

  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      it->name = ticket_name;
      it->desc = ticket_desc;
      status_code = DB_OK;
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::DeleteTicket(size_t id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      tickets.erase(it);
      status_code = DB_OK;
      return;
    }
  }

  status_code = DB_ERR;
}

Ticket DBase::GetTicket(size_t id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      return *it;
    }
  }

  status_code = DB_ERR;
  std::vector<Attachment> empty_attachment;
  std::vector<TicketTask> empty_task;
  return Ticket {0, 0, TKT_DEL, 0, "", "", empty_attachment, empty_task };
}

void DBase::MoveTicketToGroup(size_t id, size_t new_group_id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      it->group_id = new_group_id;
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::ChangeTicketState(size_t id, TicketState new_state)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      it->state = new_state;
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::AddTicketAttachment(size_t id, std::string attachment_name)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      size_t new_id = _GetTicketsAttachmentsLastId(id);
      Attachment new_att = { new_id + 1, attachment_name };
      it->attachments.push_back(new_att);
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::EditTicketAttachment
(
  size_t id,
  size_t attachment_id,
  std::string new_attachment_name
)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto att_it = it->attachments.begin(); att_it != it->attachments.end(); ++att_it)
      {
        if (att_it->id == attachment_id)
        {
          status_code = DB_OK;
          att_it->name = new_attachment_name;
          return;
        }
      }
    }
  }

  status_code = DB_ERR;
}

void DBase::DeleteTicketAttachment(size_t id, size_t attachment_id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto att_it = it->attachments.begin(); att_it != it->attachments.end(); ++att_it)
      {
        if (att_it->id == attachment_id)
        {
          status_code = DB_OK;
          it->attachments.erase(att_it);
          return;
        }
      }
    }
  }

  status_code = DB_ERR;
}

Attachment DBase::GetTicketAttachment(size_t id, size_t attachment_id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto att_it = it->attachments.begin(); att_it != it->attachments.end(); ++att_it)
      {
        if (att_it->id == attachment_id)
        {
          status_code = DB_OK;
          return *att_it;
        }
      }
    }
  }

  status_code = DB_ERR;
  return Attachment {0, ""};
}

void DBase::AddTicketTask
(
  size_t id,
  std::string desc,
  std::string action_by,
  uint64_t eta
)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      status_code = DB_OK;
      size_t new_id = _GetTicketsTasksLastId(id);
      TicketTask new_task = { new_id + 1, 0, desc, action_by, eta };
      it->tasks.push_back(new_task);
      return;
    }
  }

  status_code = DB_ERR;
}

void DBase::EditTicketTask
(
  size_t id,
  size_t task_id,
  std::string new_desc,
  std::string new_action_by,
  uint64_t new_eta
)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto tsk_it = it->tasks.begin(); tsk_it != it->tasks.end(); ++tsk_it)
      {
        if (tsk_it->id == task_id)
        {
          status_code = DB_OK;
          tsk_it->desc = new_desc;
          tsk_it->action_by = new_action_by;
          tsk_it->eta = new_eta;
          return;
        }
      }
    }
  }

  status_code = DB_ERR;
}

void DBase::SetTicketTaskProgress(size_t id, size_t task_id, uint8_t progress)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto tsk_it = it->tasks.begin(); tsk_it != it->tasks.end(); ++tsk_it)
      {
        if (tsk_it->id == task_id)
        {
          status_code = DB_OK;
          tsk_it->progress = progress;
          return;
        }
      }
    }
  }

  status_code = DB_ERR;
}

void DBase::DeleteTicketTask(size_t id, size_t task_id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto tsk_it = it->tasks.begin(); tsk_it != it->tasks.end(); ++tsk_it)
      {
        if (tsk_it->id == task_id)
        {
          status_code = DB_OK;
          it->tasks.erase(tsk_it);
          return;
        }
      }
    }
  }

  status_code = DB_ERR;
}

TicketTask DBase::GetTicketTask(size_t id, size_t task_id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == id)
    {
      for (auto tsk_it = it->tasks.begin(); tsk_it != it->tasks.end(); ++tsk_it)
      {
        if (tsk_it->id == task_id)
        {
          status_code = DB_OK;
          return *tsk_it;
        }
      }
    }
  }

  status_code = DB_ERR;
  return TicketTask {0, 0, "", "", 0};
}

std::vector<Group> DBase::GetGroups()
{
  return groups;
}

std::vector<Ticket> DBase::GetTickets(size_t group_id)
{
  std::vector<Ticket> group_tickets;

  for (auto ticket : tickets)
  {
    if (ticket.group_id == group_id)
    {
      group_tickets.push_back(ticket);
    }
  }

  return group_tickets;
}

///////////////////////////////////////////////////////////////////////////////

void DBase::_InitDB()
{
  db_path = "HOME";
  const char* home = std::getenv(db_path.c_str());

  if (!home)
  {
    status_code = DB_ERR;
    return;
  }

  db_path = std::string(home) + "/" + db_name;

  struct stat info;

  if (stat(db_path.c_str(), &info) != 0)
  {
    if (mkdir(db_path.c_str(), 0700) != 0)
    {
      status_code = DB_ERR;
      return;
    }
  }

  else if (!S_ISDIR(info.st_mode))
  {
    status_code = DB_ERR;
    return;
  }
  
  status_code = DB_OK;
}

void DBase::_LoadGroups()
{
  std::ifstream infile(db_path + "/groups.dat", std::ios::binary);

  if (!infile)
  {
    status_code = DB_ERR;
    return;
  }

  groups.clear();

  size_t num_records = 0;
    
  infile.read(reinterpret_cast<char*>(&num_records), sizeof(num_records));
  groups.reserve(num_records);

  size_t id;
  std::string name;

  while
  (
    infile.read(reinterpret_cast<char*>(&id), sizeof(id)) &&
    std::getline(infile, name, '\0')
  )
  {
    Group gr;
    gr.id = id;
    gr.name = name;

    groups.emplace_back(gr);
  }

  status_code = DB_OK;
}

void DBase::_SaveGroups()
{
  std::ofstream outfile(db_path + "/groups.dat", std::ios::binary);
    
  if (!outfile)
  {
    status_code = DB_ERR;
    return;
  }

  size_t num_records = groups.size();

  outfile.write
  (
    reinterpret_cast<const char*>(&num_records),
    sizeof(num_records)
  );

  for (const auto &gr : groups)
  {
    outfile.write(reinterpret_cast<const char*>(&gr.id), sizeof(gr.id));
    outfile.write(gr.name.c_str(), gr.name.length() + 1);
  }

  status_code = DB_OK;
}

size_t DBase::_GetGroupsLastId()
{
  if (groups.empty())
  {
    return 0;
  }
  
  auto last_group = groups.back();
  return last_group.id;
}

bool DBase::_GroupHasTickets(size_t id)
{
  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->group_id == id)
    {
      return true;
    }
  }

  return false;
}

void DBase::_LoadTickets()
{
  std::ifstream infile(db_path + "/tickets.dat", std::ios::binary);

  if (!infile)
  {
    status_code = DB_ERR;
    return;
  }

  tickets.clear();

  size_t num_records = 0;
    
  infile.read(reinterpret_cast<char*>(&num_records), sizeof(num_records));
  tickets.reserve(num_records);

  size_t id;
  size_t group_id;
  TicketState state;
  uint64_t creation_date;
  std::string name;
  std::string desc;
  std::vector<Attachment> attachments;
  std::vector<TicketTask> tasks;

  while (infile.read(reinterpret_cast<char*>(&id), sizeof(id)))
  {
    infile.read(reinterpret_cast<char*>(&group_id), sizeof(group_id));
    infile.read(reinterpret_cast<char*>(&state), sizeof(state));
    infile.read(reinterpret_cast<char*>(&creation_date), sizeof(creation_date));
    std::getline(infile, name, '\0');
    std::getline(infile, desc, '\0');

    size_t num_attachments = 0;
    infile.read(reinterpret_cast<char*>(&num_attachments), sizeof(num_attachments));
    if (num_attachments > 0)
    {
      attachments.reserve(num_attachments);
      for (size_t i = 0; i < num_attachments; i++)
      {
        Attachment attachment;
        size_t attachment_id;
        std::string attachment_name;
        infile.read(reinterpret_cast<char*>(&attachment_id), sizeof(attachment_id));
        std::getline(infile, attachment_name, '\0');
        attachment.id = attachment_id;
        attachment.name = attachment_name;
        attachments.emplace_back(attachment);
      }
    }

    size_t num_tasks = 0;
    infile.read(reinterpret_cast<char*>(&num_tasks), sizeof(num_tasks));
    if (num_tasks > 0)
    {
      tasks.reserve(num_tasks);
      for (size_t i = 0; i < num_tasks; i++)
      {
        TicketTask task;
        size_t task_id;
        uint8_t task_progress;
        std::string task_desc;
        std::string task_action_by;
        uint64_t task_eta;
        infile.read(reinterpret_cast<char*>(&task_id), sizeof(task_id));
        infile.read(reinterpret_cast<char*>(&task_progress), sizeof(task_progress));
        std::getline(infile, task_desc, '\0');
        std::getline(infile, task_action_by, '\0');
        infile.read(reinterpret_cast<char*>(&task_eta), sizeof(task_eta));
        task.id = task_id;
        task.progress = task_progress;
        task.desc = task_desc;
        task.action_by = task_action_by;
        task.eta = task_eta;
        tasks.emplace_back(task);
      }
    }

    Ticket ticket;
    ticket.id = id;
    ticket.group_id = group_id;
    ticket.state = state;
    ticket.creation_date = creation_date;
    ticket.name = name;
    ticket.desc = desc;
    ticket.attachments = attachments;
    ticket.tasks = tasks;

    tickets.emplace_back(ticket);
  }

  status_code = DB_OK;
}

void DBase::_SaveTickets()
{
  std::ofstream outfile(db_path + "/tickets.dat", std::ios::binary);
    
  if (!outfile)
  {
    status_code = DB_ERR;
    return;
  }

  size_t num_records = tickets.size();

  outfile.write
  (
    reinterpret_cast<const char*>(&num_records),
    sizeof(num_records)
  );

  for (const auto &ticket : tickets)
  {
    outfile.write(reinterpret_cast<const char*>(&ticket.id), sizeof(ticket.id));
    outfile.write(reinterpret_cast<const char*>(&ticket.group_id), sizeof(ticket.group_id));
    outfile.write(reinterpret_cast<const char*>(&ticket.state), sizeof(ticket.state));
    outfile.write(reinterpret_cast<const char*>(&ticket.creation_date), sizeof(ticket.creation_date));
    outfile.write(ticket.name.c_str(), ticket.name.length() + 1);
    outfile.write(ticket.desc.c_str(), ticket.desc.length() + 1);

    size_t num_attachments = ticket.attachments.size();
    outfile.write(reinterpret_cast<const char*>(&num_attachments), sizeof(num_attachments));
    for (const auto &attachment : ticket.attachments)
    {
      outfile.write(reinterpret_cast<const char*>(&attachment.id), sizeof(attachment.id));
      outfile.write(attachment.name.c_str(), attachment.name.length() + 1);
    }

    size_t num_tasks = ticket.tasks.size();
    outfile.write(reinterpret_cast<const char*>(&num_tasks), sizeof(num_tasks));
    for (const auto &task : ticket.tasks)
    {
      outfile.write(reinterpret_cast<const char*>(&task.id), sizeof(task.id));
      outfile.write(reinterpret_cast<const char*>(&task.progress), sizeof(task.progress));
      outfile.write(task.desc.c_str(), task.desc.length() + 1);
      outfile.write(task.action_by.c_str(), task.action_by.length() + 1);
      outfile.write(reinterpret_cast<const char*>(&task.eta), sizeof(task.eta));
    }
  }

  status_code = DB_OK;
}

size_t DBase::_GetTicketsLastId()
{
  if (tickets.empty())
  {
    return 0;
  }

  auto last_ticket = tickets.back();
  return last_ticket.id;
}

size_t DBase::_GetTicketsAttachmentsLastId(size_t ticket_id)
{
  status_code = DB_OK;

  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == ticket_id)
    {
      if (it->attachments.empty())
      {
        return 0;
      }

      auto last_attachment = it->attachments.back();
      return last_attachment.id;
    }
  }

  status_code = DB_ERR;
  return 0;
}

size_t DBase::_GetTicketsTasksLastId(size_t ticket_id)
{
  status_code = DB_OK;

  for (auto it = tickets.begin(); it != tickets.end(); ++it)
  {
    if (it->id == ticket_id)
    {
      if (it->tasks.empty())
      {
        return 0;
      }

      auto last_task = it->tasks.back();
      return last_task.id;
    }
  }

  status_code = DB_ERR;
  return 0;
}
