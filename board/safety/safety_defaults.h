timestamp_t audi_time = {0};

const addr_checks default_rx_checks = {
  .check = NULL,
  .len = 0,
};

int default_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  int addr = GET_ADDR(to_push);

  if (addr == 0x6B2)
  {
    audi_time.year = (((GET_BYTE(to_push, 4) << 4) | (GET_BYTE(to_push, 3) >> 4)) & 0x7F) + 2000;
    audi_time.month = (GET_BYTE(to_push, 4) >> 3) & 0x0F;
    audi_time.day = ((GET_BYTE(to_push, 5) << 1) | (GET_BYTE(to_push, 4) >> 7)) & 0x1F;
    audi_time.hour = ((GET_BYTE(to_push, 6) << 4) | (GET_BYTE(to_push, 5) >> 4)) & 0x1F;
    audi_time.minute = (GET_BYTE(to_push, 6) >> 1) & 0x3F;
    audi_time.second = ((GET_BYTE(to_push, 7) << 1) | (GET_BYTE(to_push, 6) >> 7)) & 0x3F;
  }

  return true;
}

// *** no output safety mode ***

static const addr_checks* nooutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = false;
  relay_malfunction_reset();
  return &default_rx_checks;
}

static int nooutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return false;
}

static int nooutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return false;
}

static int default_fwd_hook(int bus_num, CAN_FIFOMailBox_TypeDef *to_fwd) {
  UNUSED(bus_num);
  UNUSED(to_fwd);
  return -1;
}

const safety_hooks nooutput_hooks = {
  .init = nooutput_init,
  .rx = default_rx_hook,
  .tx = nooutput_tx_hook,
  .tx_lin = nooutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};

// *** all output safety mode ***

static const addr_checks* alloutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = true;
  relay_malfunction_reset();
  return &default_rx_checks;
}

static int alloutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return true;
}

static int alloutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return true;
}

const safety_hooks alloutput_hooks = {
  .init = alloutput_init,
  .rx = default_rx_hook,
  .tx = alloutput_tx_hook,
  .tx_lin = alloutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};
