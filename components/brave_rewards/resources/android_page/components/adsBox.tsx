/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import BoxMobile, { Props as BoxMobileProps } from '../../ui/components/mobile/boxMobile'
import { List, NextContribution, Tokens } from '../../ui/components'
import { Grid, Column, Select, ControlWrapper, Checkbox } from 'brave-ui/components'
import AdsOnboarding from './adsOnboarding'
import {
  StyledListContent,
  StyledTotalContent
} from './style'

// Utils
import { getLocale } from '../../../../common/locale'
import * as rewardsActions from '../actions/rewards_actions'
import * as utils from '../utils'

interface Props extends Rewards.ComponentProps {
}

class AdsBox extends React.Component<Props, {}> {
  constructor (props: Props) {
    super(props)
  }

  onAdsSettingChange = (key: string, value: boolean) => {
    let newValue: any = value
    const {
      adsEnabled,
      shouldAllowSubdivisionAdTargeting
    } = this.props.rewardsData.adsData

    if (key === 'adsEnabled') {
      newValue = !adsEnabled
    } else if (key === 'shouldAllowSubdivisionAdTargeting') {
      newValue = !shouldAllowSubdivisionAdTargeting
    } else if (key == 'adsSubdivision') {
      this.props.actions.onAdsSettingSave('didOverrideAdsSubdivision', !didOverrideAdsSubdivision);
    }

    this.props.actions.onAdsSettingSave(key, newValue)
  }

  adsSettings = (enabled?: boolean) => {
    if (!enabled) {
      return null
    }

    const {
      adsPerHour,
      shouldAllowSubdivisionAdTargeting,
      adsSubdivision,
      isSubdivisionAdTargetingRegion
    } = this.props.rewardsData.adsData

    return (
      <Grid columns={1}>
        <Column size={1} customStyle={{ justifyContent: 'center', flexWrap: 'wrap' }}>
          <ControlWrapper text={getLocale('adsPerHour')}>
            <Select
              value={adsPerHour.toString()}
              onChange={this.onAdsSettingChange.bind(this, 'adsPerHour')}
            >
              {['1', '2', '3', '4', '5'].map((num: string) => {
                return (
                  <div key={`num-per-hour-${num}`} data-value={num}>
                  {getLocale(`adsPerHour${num}`)}
                  </div>
                )
              })}
            </Select>
          </ControlWrapper>
        </Column>

        <div style={{display: isSubdivisionAdTargetingRegion ? 'block' : 'none' }}>
          <Column size={1} customStyle={{ justifyContent: 'center', flexWrap: 'wrap' }}>
            <ControlWrapper text={getLocale('adsAllowSubdivisionTargetingTitle')}>
              <Checkbox
                value={{
                  shouldAllowSubdivisionAdTargeting: shouldAllowSubdivisionAdTargeting
                }}
                multiple={true}
                onChange={this.onAdsSettingChange}
              >
                <div data-key='shouldAllowSubdivisionAdTargeting'>{getLocale('adsAllowSubdivisionTargetingLabel')}</div>
              </Checkbox>

              <Select
                value={(adsSubdivision || '').toString()}
                onChange={this.onAdsSettingChange.bind(this, 'adsSubdivision')}
              >
                {[['US-AL', 'Alabama'], ['US-AK', 'Alaska'], ['US-AZ', 'Arizona'], ['US-AR', 'Arkansas'], ['US-CA', 'California'], ['US-CO', 'Colorado'], ['US-CT', 'Connecticut'], ['US-DE', 'Delaware'], ['US-FL', 'Florida'], ['US-GA', 'Georgia'], ['US-HI', 'Hawaii'], ['US-ID', 'Idaho'], ['US-IL', 'Illinois'], ['US-IN', 'Indiana'], ['US-IA', 'Iowa'], ['US-KS', 'Kansas'], ['US-KY', 'Kentucky'], ['US-LA', 'Louisiana'], ['US-ME', 'Maine'], ['US-MD', 'Maryland'], ['US-MA', 'Massachusetts'], ['US-MI', 'Michigan'], ['US-MN', 'Minnesota'], ['US-MS', 'Mississippi'], ['US-MO', 'Missouri'], ['US-MT', 'Montana'], ['US-NE', 'Nebraska'], ['US-NV', 'Nevada'], ['US-NH', 'New Hampshire'], ['US-NJ', 'New Jersey'], ['US-NM', 'New Mexico'], ['US-NY', 'New York'], ['US-NC', 'North Carolina'], ['US-ND', 'North Dakota'], ['US-OH', 'Ohio'], ['US-OK', 'Oklahoma'], ['US-OR', 'Oregon'], ['US-PA', 'Pennsylvania'], ['US-RI', 'Rhode Island'], ['US-SC', 'South Carolina'], ['US-SD', 'South Dakota'], ['US-TN', 'Tennessee'], ['US-TX', 'Texas'], ['US-UT', 'Utah'], ['US-VT', 'Vermont'], ['US-VA', 'Virginia'], ['US-WA', 'Washington'], ['US-WV', 'West Virginia'], ['US-WI', 'Wisconsin'], ['US-WY', 'Wyoming']].map((subdivision: Array<String>) => {
                  return (
                    <div key={`${subdivision[0]}`} data-value={subdivision[0]}>
                    {`${subdivision[1]}`}
                    </div>
                  )
                })}
              </Select>
            </ControlWrapper>
          </Column>

          <div>{getLocale('adsAllowSubdivisionTargetingDescription')} <a href={'#'} target={'_blank'}>{getLocale('adsAllowSubdivisionTargetingLearn')}</a></div>
        </div>
      </Grid>
    )
  }

  render () {
    // Default values from storage.ts
    let adsEnabled = false
    let adsUIEnabled = false
    let adsIsSupported = false
    let estimatedPendingRewards = 0
    let nextPaymentDate = ''
    let adNotificationsReceivedThisMonth = 0
    const {
      adsData,
      enabledMain,
      balance,
      safetyNetFailed
    } = this.props.rewardsData
    const { onlyAnonWallet } = this.props.rewardsData.ui

    if (adsData) {
      adsEnabled = adsData.adsEnabled
      adsUIEnabled = adsData.adsUIEnabled
      adsIsSupported = adsData.adsIsSupported
      estimatedPendingRewards = adsData.adsEstimatedPendingRewards || 0
      nextPaymentDate = adsData.adsNextPaymentDate
      adNotificationsReceivedThisMonth = adsData.adsAdNotificationsReceivedThisMonth || 0
    }

    // disabled / alert state
    const isDisabled = safetyNetFailed || !adsIsSupported || !adsUIEnabled
    const toggle = !isDisabled && enabledMain
    // Sanity-check: ensure no action can be performed if the box isn't allowed
    // to be enabled
    const toggleAction = toggle
      ? this.onAdsSettingChange.bind(this, 'adsEnabled', '')
      : () => undefined
    const boxPropsExtra: Partial<BoxMobileProps> = {
      toggle,
      toggleAction,
      checked: toggle && adsEnabled
    }
    if (isDisabled) {
      boxPropsExtra.alertContent = safetyNetFailed
         ? <>{getLocale('adsNotSupportedDevice')}</>
         : !adsIsSupported
           ? <>{getLocale('adsNotSupportedRegion')}</>
           : <>This version of Brave does not support Ads.</>
    }
    if (!isDisabled && !boxPropsExtra.checked) {
      boxPropsExtra.extraDescriptionChild = <AdsOnboarding />
    }

    const tokenString = getLocale(onlyAnonWallet ? 'points' : 'tokens')

    return (
      <BoxMobile
        title={getLocale('adsTitle')}
        type={'ads'}
        description={getLocale('adsDesc', { currency : tokenString })}
        settingsChild={this.adsSettings(adsEnabled && enabledMain)}
        {...boxPropsExtra}
      >
        <List title={<StyledListContent>{getLocale('adsCurrentEarnings')}</StyledListContent>}>
          <StyledTotalContent>
            <Tokens
              onlyAnonWallet={onlyAnonWallet}
              value={estimatedPendingRewards.toFixed(1)}
              converted={utils.convertBalance(estimatedPendingRewards, balance.rates)}
            />
          </StyledTotalContent>
        </List>
        <List title={<StyledListContent>{getLocale('adsPaymentDate')}</StyledListContent>}>
          <StyledListContent>
            <NextContribution>
              {nextPaymentDate}
            </NextContribution>
          </StyledListContent>
        </List>
        <List title={<StyledListContent>{getLocale('adsNotificationsReceived')}</StyledListContent>}>
          <StyledListContent>
            <Tokens
              onlyAnonWallet={onlyAnonWallet}
              value={adNotificationsReceivedThisMonth.toString()}
              hideText={true}
            />
          </StyledListContent>
        </List>
      </BoxMobile>
    )
  }
}

const mapStateToProps = (state: Rewards.ApplicationState) => ({
  rewardsData: state.rewardsData
})

const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(AdsBox)
